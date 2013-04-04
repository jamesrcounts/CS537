#include <cstring>
#include <fstream>
#include <iostream>
#include <igloo/igloo.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../Namer.h"
#include "../StringWriter.h"
#include "../reporters/Reporter.h"

using namespace igloo;
using namespace std;

class ApprovalException : public std::exception
{
private:
    char *message;
protected:
    char *init( const char *msg )
    {
        size_t message_sz = std::strlen( msg );
        char *t = new char[message_sz + 1];
        std::strncpy( t, msg, message_sz + 1 );
        return t;
    };
public:
    ApprovalException( const char *msg ) : message( init( msg ) ) {}

    ApprovalException( const ApprovalException &a )
        : message( init( a.message ) )
    {
    }

    virtual ~ApprovalException() throw()
    {
        delete[] message;
    }

    virtual const char *what() const throw()
    {
        return message;
    }
};

class ApprovalMismatchException : public ApprovalException
{
private:
    char *format( const std::string &received, const std::string &approved )
    {
        size_t n = 2048;
        char s[n];
        int size = snprintf( s,
                             n,
                             "Failed Approval: Received file %s does not match approved file %s.",
                             received.c_str(),
                             approved.c_str() );
        char *t = new char[size + 1];
        std::strncpy( t, s, size + 1 );
        return t;
    }
public:
    ApprovalMismatchException( std::string received, std::string approved )
        : ApprovalException( format( received, approved ) )
    {
    }

    ApprovalMismatchException( const ApprovalMismatchException &a )
        : ApprovalException( a ) {}
};

class ApprovalMissingException : public ApprovalException
{
private:
    char *format( const std::string &file )
    {
        size_t n = 1024;
        char s[n];
        int size = snprintf( s,
                             n,
                             "Failed Approval: Approval File \"%s\" Not Found.",
                             file.c_str() );
        char *t = new char[size + 1];
        std::strncpy( t, s, size + 1 );
        return t;
    }
public:
    ApprovalMissingException( std::string received, std::string approved )
        : ApprovalException( format( approved ) )
    {
    }

    ApprovalMissingException( const ApprovalMissingException &a )
        : ApprovalException( a )
    {
    }
};

class FileApprover
{
protected:
    FileApprover() {};
    ~FileApprover() {};

    static ApprovalException *verify( std::string receivedPath,
                                      std::string approvedPath )
    {
        int asize = fileSize( approvedPath );

        if ( -1 == asize )
        {
            return new ApprovalMissingException( receivedPath, approvedPath );
        }

        int rsize = fileSize( receivedPath );

        if ( -1 == rsize )
        {
            return new ApprovalMissingException( approvedPath, receivedPath );
        }

        if ( asize != rsize )
        {
            return new ApprovalMismatchException( receivedPath, approvedPath );
        }

        ifstream astream( approvedPath.c_str(), ios::binary | ifstream::in );
        ifstream rstream( receivedPath.c_str(), ios::binary | ifstream::in );

        while ( astream.good() && rstream.good() )
        {
            int a = astream.get();
            int r = rstream.get();

            if ( a != r )
            {
                return new ApprovalMismatchException( receivedPath, approvedPath );
            }
        }

        remove( receivedPath );
        return NULL;
    }

public:
    static bool fileExists( std::string path )
    {
        return fileSize( path ) != -1;
    }

    static int fileSize( std::string path )
    {
        struct stat statbuf;
        int stat_ok = stat( path.c_str(), &statbuf );

        if ( stat_ok == -1 )
        {
            return -1;
        }

        return int( statbuf.st_size );
    }

    static void verify( Namer n, StringWriter s, Reporter r )
    {
        std::string approvedPath = n.getApprovedFile( ".txt" );
        std::string receivedPath = n.getReceivedFile( ".txt" );
        s.write( receivedPath );
        ApprovalException *ae = verify( receivedPath, approvedPath );

        if ( ae != NULL )
        {
            r.report( receivedPath, approvedPath );
            ApprovalException e( *ae );
            delete ae;
            throw e;
        }
    }
};

Context( DescribeApprovalExceptions )
{
    Spec( ApprovalMissingExceptionHasAMessage )
    {
        ApprovalMissingException a( "r.txt", "a.txt" );
        Assert::That( a.what(),
                      Equals( "Failed Approval: Approval File \"a.txt\" Not Found." ) );
    }

    Spec( ApprovalMismatchExceptionHasAMessage )
    {
        ApprovalMismatchException a( "r.txt", "a.txt" );
        Assert::That( a.what(),
                      Equals( "Failed Approval: Received file r.txt does not match approved file a.txt." ) );
    }
};

Context( DescribeAFileApprover )
{
    string bin()
    {
        char buf[512];
        bzero( buf, 512 );
        ssize_t readlink_ok = readlink( "/proc/self/cwd", buf, 512 );
        Assert::That( readlink_ok, IsGreaterThan( -1 ) );
        string dir( buf );
        return dir;
    }

    void writeMessageTo( string message, string path )
    {
        ofstream a( path.c_str(), ofstream::out );
        a << message;
        a.close();
    }

    Spec( ItVerifiesApprovedFileExists )
    {
        Namer namer( bin(),
                     "DescribeAFileApprover.ItVerifiesApprovedFileExists" );
        StringWriter writer( "Hello" );
        TestReporter reporter;

        string approved = namer.getApprovedFile( ".txt" );
        string received = namer.getReceivedFile( ".txt" );

        string expected =  "Failed Approval: Approval File \"" +
                           approved +
                           "\" Not Found.";

        AssertThrows( ApprovalException,
                      FileApprover::verify( namer, writer, reporter ) );
        AssertThat( LastException<ApprovalException>().what(),
                    Equals( expected ) );

        remove( approved );
        remove( received );
    }

    Spec( ItVerifiesFilesHaveEqualSize )
    {
        Namer namer( bin(),
                     "DescribeAFileApprover.ItVerifiesFilesHaveEqualSize" );
        StringWriter writer( "Hello" );
        TestReporter reporter;

        string approved = namer.getApprovedFile( ".txt" );
        string received = namer.getReceivedFile( ".txt" );

        writeMessageTo( "Hello World", approved );

        string expected =  "Failed Approval: Received file " +
                           received +
                           " does not match approved file " +
                           approved +
                           ".";

        AssertThrows( ApprovalException,
                      FileApprover::verify( namer, writer, reporter ) );
        AssertThat( LastException<ApprovalException>().what(),
                    Equals( expected ) );

        remove( approved );
        remove( received );
    }

    Spec( ItVerifiesEveryByteIsEqual )
    {
        Namer namer( bin(),
                     "DescribeAFileApprover.ItVerifiesEveryByteIsEqual" );
        StringWriter writer( "Hello" );
        TestReporter reporter;

        string approved = namer.getApprovedFile( ".txt" );
        string received = namer.getReceivedFile( ".txt" );

        writeMessageTo( "olleH", approved );

        string expected =  "Failed Approval: Received file " +
                           received +
                           " does not match approved file " +
                           approved +
                           ".";

        AssertThrows( ApprovalException,
                      FileApprover::verify( namer, writer, reporter ) );
        AssertThat( LastException<ApprovalException>().what(),
                    Equals( expected ) );

        remove( approved );
        remove( received );
    }

    Spec( ItLaunchesReporterOnFailure )
    {
        Namer namer( bin(),
                     "DescribeAFileApprover.ItLaunchesReporterOnFailure" );
        string approved = namer.getApprovedFile( ".txt" );
        string received = namer.getReceivedFile( ".txt" );
        writeMessageTo( "olleH", approved );
        StringWriter writer( "Hello" );
        TestReporter reporter;

        string expected = "fake " + received + " " + approved + " ";
        AssertThrows( ApprovalException,
                      FileApprover::verify( namer, writer, reporter ) );
        AssertThat( reporter.launcher.receivedCommand(),
                    Equals( expected ) );

        remove( approved );
        remove( received );
    }

    Spec( ItApprovesMatchingFiles )
    {
        Namer namer( bin(),
                     "DescribeAFileApprover.ItApprovesMatchingFiles" );
        string approved = namer.getApprovedFile( ".txt" );
        string received = namer.getReceivedFile( ".txt" );
        writeMessageTo( "Hello", approved );
        StringWriter writer( "Hello" );
        TestReporter reporter;

        string expected = "";
        FileApprover::verify( namer, writer, reporter );
        AssertThat( reporter.launcher.receivedCommand(),
                    Equals( expected ) );

        remove( approved );
        remove( received );
    }

    Spec( ItCanTellYouThatAFileExists )
    {
        Namer namer( bin(),
                     "DescribeAFileApprover.ItCanTellYouThatAFileExists" );
        string approved = namer.getApprovedFile( ".txt" );
        writeMessageTo( "Hello", approved );
        bool exists = FileApprover::fileExists( approved );

        struct stat statbuf;
        bool stat_ok = ( stat( approved.c_str(), &statbuf ) != -1 );

        Assert::That( exists, Equals( stat_ok ) );
        remove( approved );
    }

    Spec( ItCanTellYouTheFileSize )
    {
        Namer namer( bin(),
                     "DescribeAFileApprover.ItCanTellYouTheFileSize" );
        string approved = namer.getApprovedFile( ".txt" );
        writeMessageTo( "Hello", approved );
        int size = FileApprover::fileSize( approved );

        struct stat statbuf;
        bool stat_ok = ( stat( approved.c_str(), &statbuf ) != -1 );
        AssertThat( stat_ok, Equals( true ) );
        AssertThat( size, Equals( ( int )statbuf.st_size ) );
        remove( approved );
    }

    Spec( ItRemovesReceivedFileOnApproval )
    {
        Namer namer( bin(),
                     "DescribeAFileApprover.ItRemovesReceivedFileOnApproval" );
        string approved = namer.getApprovedFile( ".txt" );
        string received = namer.getReceivedFile( ".txt" );
        writeMessageTo( "Hello", approved );
        StringWriter writer( "Hello" );
        TestReporter reporter;

        FileApprover::verify( namer, writer, reporter );
        Assert::That( FileApprover::fileExists( received ),
                      Equals( false ) );

        remove( approved );
    }

    Spec( ItPreservesApproveFileOnApproval )
    {
        Namer namer( bin(),
                     "DescribeAFileApprover.ItPreservesApproveFileOnApproval" );
        string approved = namer.getApprovedFile( ".txt" );
        string received = namer.getReceivedFile( ".txt" );
        writeMessageTo( "Hello", approved );
        StringWriter writer( "Hello" );
        TestReporter reporter;

        FileApprover::verify( namer, writer, reporter );
        Assert::That( FileApprover::fileExists( approved ),
                      Equals( true ) );

        remove( approved );
    }
};
/*
describe('FileApprover', function () {

	describe('', function () {
		it('should verify two files match', function () {

			var writer = new StringWriter(config, "HELLO!");
			var reporter = new DoNothingReporter();

			FileApprover.verify(namer, writer, reporter);

		});
	});
});
*/
int main( int argc, char const *argv[] )
{
    return TestRunner::RunAllTests();
}