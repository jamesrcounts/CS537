/* test.cpp */

#include <igloo/igloo.h>
#include <sys/stat.h>
#include <sstream>
#include "HttpHandler.h"

using namespace igloo;
using namespace std;

Context( GatheringInformation )
{
    Spec( TestForExistence )
    {
        AssertThat( HttpHandler::FileExists( "foo" ), Equals( false ) );
        AssertThat( HttpHandler::FileExists( "test.cpp" ), Equals( true ) );
    }

    Spec( GetContentLength )
    {
        string header( "PUT /log2.txt HTTP/1.0" );
        header += "\nHost: 127.0.0.1:8901";
        header += "\nContent-Length: 657\n";
        int expected = 657;
        int actual = HttpHandler::StreamLength( header );
        Assert::That( actual, Equals( expected ) );
    }

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
    Spec( WriteBadRequestStatus )
    {
        string expected( "HTTP/1.0 400 Bad Request\n" );
        string actual = HttpHandler::StatusBadRequest( "HTTP/1.0" );
        Assert::That( actual, Equals( expected ) );
    }
    Spec( WriteNotFoundStatus )
    {
        string expected( "HTTP/1.0 404 Not Found\n" );
        string actual = HttpHandler::StatusNotFound( "HTTP/1.0" );
        Assert::That( actual, Equals( expected ) );
    }

    Spec( WriteErrorStatus )
    {
        string expected( "HTTP/1.0 500 Internal Server Error\n" );
        string actual = HttpHandler::StatusInternalError( "HTTP/1.0" );
        Assert::That( actual, Equals( expected ) );
    }

    Spec( WriteOKStatus )
    {
        string expected( "HTTP/1.0 200 OK\n" );
        string actual = HttpHandler::StatusOk( "HTTP/1.0" );
        Assert::That( actual, Equals( expected ) );
    }

    Spec( WriteContentLengthHeader )
    {
        string path( "test.cpp" );
        string expected( "Content-Length: " );
        expected += HttpHandler::SizeOf( path );
        expected += "\n";
        string actual = HttpHandler::ContentLengthHeader( path );
        Assert::That( actual, Equals( expected ) );
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