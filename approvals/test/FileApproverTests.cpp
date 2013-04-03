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

class ApprovalMismatchException
{
private:
    char *message;
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
        : message( format( received, approved ) )
    {
    }


    ApprovalMismatchException( const ApprovalMismatchException &a )
        : message( NULL )
    {
        size_t message_sz = std::strlen( a.message );
        message = new char[message_sz + 1];
        std::strncpy( message, a.message, message_sz + 1 );
    }

    ~ApprovalMismatchException() throw()
    {
        delete[] message;
    }

    virtual const char *what() const throw()
    {
        return message;
    }
};

class ApprovalMissingException : public std::exception
{
private:
    char *message;

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
        : message( format( approved ) )
    {
    }

    ApprovalMissingException( const ApprovalMissingException &a )
        : message( NULL )
    {
        size_t message_sz = std::strlen( a.message );
        message = new char[message_sz + 1];
        std::strncpy( message, a.message, message_sz + 1 );
    }

    ~ApprovalMissingException() throw()
    {
        delete[] message;
    }

    virtual const char *what() const throw()
    {
        return message;
    }
};

class FileApprover
{
protected:
    FileApprover();
    ~FileApprover();
public:
    static void verify( Namer n, StringWriter s, Reporter r )
    {
        std::string approvedPath = n.getApprovedFile( ".txt" );
        std::string receivedPath = n.getReceivedFile( ".txt" );

        struct stat astatbuf;
        int stat_ok = stat( approvedPath.c_str(), &astatbuf );

        if ( stat_ok == -1 )
        {
            throw ApprovalMissingException( receivedPath, approvedPath );
        }

        s.write( receivedPath );
        struct stat rstatbuf;

        if ( -1 == stat( receivedPath.c_str(), &rstatbuf ) )
        {
            throw ApprovalMissingException( approvedPath, receivedPath );
        }

        if ( astatbuf.st_size != rstatbuf.st_size )
        {
            throw ApprovalMismatchException( receivedPath, approvedPath );
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

    Spec( ItVerifiesApprovedFileExists )
    {
        Namer namer( bin(),
                     "DescribeAFileApprover.ItVerifiesApprovedFileExists" );
        StringWriter writer( "Hello" );
        TestReporter reporter;
        AssertThrows( ApprovalMissingException,
                      FileApprover::verify( namer, writer, reporter ) );
    }

    Spec( ItVerifiesFilesHaveEqualSize )
    {
        Namer namer( bin(),
                     "DescribeAFileApprover.ItVerifiesFilesHaveEqualSize" );

        ofstream a( namer.getApprovedFile( ".txt" ).c_str(), ofstream::out );
        a << "Hello World" << endl;
        a.close();

        StringWriter writer( "Hello" );
        TestReporter reporter;
        AssertThrows( ApprovalMismatchException,
                      FileApprover::verify( namer, writer, reporter ) );

        remove( namer.getApprovedFile( ".txt" ) );
        remove( namer.getReceivedFile( ".txt" ) );
    }

    Spec( ItVerifiesEveryByteIsEqual ) {}
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