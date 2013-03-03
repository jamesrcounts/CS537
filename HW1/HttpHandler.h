#ifndef HTTPHANDLER_H
#define HTTPHANDLER_H

#include <sstream>

using namespace std;

class HttpHandler
{
public:
    HttpHandler();
    ~HttpHandler();

    static string BlankLine()
    {
        return "\n";
    }

    static string ContentLengthHeader( string path )
    {
        string message( "Content-Length: " );
        message += SizeOf( path );
        message += "\n";
        return message;
    }

    static string FixupPath( string path )
    {
        return "." + path;
    }

    static string LastModified( string path )
    {
        int buffer_sz = 80;
        char format[buffer_sz];
        struct stat statbuf;

        int stat_result = stat( path.c_str(), &statbuf );

        if ( stat_result != -1 )
        {
            struct tm *timeinfo = localtime( &statbuf.st_mtime );
            strftime( format, buffer_sz, "%c", timeinfo );
        }

        return string( format );
    }

    static string LastModifiedHeader( string path )
    {
        string message( "Last-Modified: " );
        message += LastModified( path );
        message += "\n";
        return message;
    }

    static string ServerHeader()
    {
        return "Server: myhttpd/1.0\n";
    }

    static string SizeOf( string path )
    {
        struct stat statbuf;
        int stat_result =  stat( path.c_str(), &statbuf ) ;

        stringstream sz;

        if ( stat_result != -1 )
        {
            sz <<  statbuf.st_size;
        }

        return sz.str();
    }
};
#endif