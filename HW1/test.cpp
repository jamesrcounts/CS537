/* test.cpp */

#include <igloo/igloo.h>
#include <sys/stat.h>
#include <sstream>
#include "HttpHandler.h"

using namespace igloo;
using namespace std;

Context( GatheringInformation )
{
    Spec( GetFileSize )
    {
        const char *path = "test.cpp";
        struct stat statbuf;
        int stat_result =  stat( path, &statbuf ) ;
        AssertThat( stat_result, !Equals( -1 ) );

        stringstream ex;
        ex <<  statbuf.st_size;

        string actual = HttpHandler::SizeOf( string( path ) );
        AssertThat( actual, Equals( ex.str() ) );
    }


    Spec( GetFilesLastModifiedTime )
    {
        const char *path = "test.cpp";
        struct stat statbuf;
        int stat_result = stat( path, &statbuf );
        AssertThat( stat_result, !Equals( -1 ) );

        char ex[80];
        struct tm *timeinfo = localtime( &statbuf.st_mtime );
        strftime( ex, 80, "%c", timeinfo );

        string expected( ex );
        string actual = HttpHandler::LastModified( string( path ) );
        AssertThat( actual, Equals( expected ) );
    }
};

Context( RespondingToRequests )
{
    Spec( WriteContentLengthHeader )
    {
        string path( "test.cpp" );
        string expected( "Content-Length: " );
        expected += HttpHandler::SizeOf( path );
        expected += "\n";
        string actual = HttpHandler::ContentLengthHeader( path );
        Assert::That( expected, Equals( actual ) );
    }
    Spec( WriteLastModifiedHeader )
    {
        string path( "test.cpp" );
        string expected( "Last-Modified: " );
        expected += HttpHandler::LastModified( path );
        expected += "\n";

        string actual = HttpHandler::LastModifiedHeader( path );
        Assert::That( actual, Equals( expected ) );
    }

    Spec( WriteBlankLine )
    {
        string expected( "\n" );
        string actual = HttpHandler::BlankLine();
        Assert::That( actual, Equals( expected ) );
    }

    Spec( WriteTheServerHeader )
    {
        string expected( "Server: myhttpd/1.0\n" );
        string actual = HttpHandler::ServerHeader();
        Assert::That( actual, Equals( expected ) );
    }
};

Context( FixingAPath )
{
    Spec( AssumeAllPathsAreRelative )
    {
        string expected( "./log.txt" );
        string actual = HttpHandler::FixupPath( "/log.txt" );
        Assert::That( actual, Equals( expected ) );
    }
};

int main( int argc, char const *argv[] )
{
    return TestRunner::RunAllTests();
}