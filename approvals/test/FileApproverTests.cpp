#include <cstring>
#include <fstream>
#include <iostream>
#include <igloo/igloo.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../ApprovalException.h"
#include "../FileApprover.h"
#include "../StringWriter.h"
#include "../reporters/Reporter.h"

using namespace igloo;
using namespace std;

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

        remove( approved.c_str() );
        remove( received.c_str() );
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

        remove( approved.c_str() );
        remove( received.c_str() );
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

        remove( approved.c_str() );
        remove( received.c_str() );
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

        remove( approved.c_str() );
        remove( received.c_str() );
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

        remove( approved.c_str() );
        remove( received.c_str() );
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
        remove( approved.c_str() );
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
        remove( approved.c_str() );
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

        remove( approved.c_str() );
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

        remove( approved.c_str() );
    }
};

int main( int argc, char const *argv[] )
{
    return TestRunner::RunAllTests();
}