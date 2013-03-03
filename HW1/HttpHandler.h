#ifndef HTTPHANDLER_H
#define HTTPHANDLER_H

#include <fstream>
#include <sstream>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

using namespace std;

#define MAXCHUNK 32768

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

    static string Delete( string protocol, string path )
    {
        path = FixupPath( path );

        string response  = StatusInternalError( protocol ) ;

        if ( remove( path.c_str() ) == 0 )
        {
            response = StatusOk( protocol ) ;
        }

        return response + BlankLine();
    }

    static bool FileExists( string path )
    {
        struct stat statbuf;
        return stat( path.c_str(), &statbuf ) != -1;
    }

    static string FixupPath( string path )
    {
        return "." + path;
    }

    static string Get( int fd, string protocol, string path )
    {
        string response = Head( protocol, path );
        write( fd, response.c_str(), response.size() );

        path = FixupPath( path );

        if ( FileExists( path ) )
        {
            WriteFileToStream( fd, path );
        }

        return "";
    }

    static string Head( string protocol, string path )
    {
        string response = StatusNotFound( protocol );

        path = FixupPath( path );

        if ( FileExists( path ) )
        {
            response =  StatusOk( protocol )  +
                        ServerHeader() +
                        LastModifiedHeader( path ) +
                        ContentLengthHeader( path );
        }

        return response + BlankLine();

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

    static string Put( string protocol, string header, string path, int fd )
    {
        string response = StatusOk( protocol );

        int length = StreamLength( header );

        if ( length != 0 )
        {
            WriteFile( FixupPath( path ), ReadStream( fd, length ) );
        }
        else
        {
            response = StatusBadRequest( protocol );
        }

        return response + BlankLine();
    }

    static string ReadStream( int fd, int size )
    {
        int chunk_sz = ( size < MAXCHUNK ) ? size : MAXCHUNK;
        char str[chunk_sz];

        string contents;

        while ( contents.size() < size )
        {
            bzero( str, chunk_sz );
            int read_bytes = read( fd, str, chunk_sz - 1 );

            if ( read_bytes )
            {
                contents += str;
            }
        }

        return contents;
    }

    static string ServerHeader()
    {
        return "Server: myhttpd/1.0\n";
    }

    static string StatusBadRequest( string protocol )
    {
        return protocol + " 400 Bad Request\n";
    }

    static string StatusInternalError( string protocol )
    {
        return protocol + " 500 Internal Server Error\n";
    }

    static string StatusNotFound( string protocol )
    {
        return protocol + " 404 Not Found\n";
    }

    static string StatusOk( string protocol )
    {
        return protocol + " 200 OK\n";
    }

    static int StreamLength( string header )
    {
        int length = 0;
        char line[256];
        stringstream buffer( header );

        while ( buffer.getline( line, 256 ) )
        {
            string name;
            stringstream line_buffer( line );

            line_buffer >> name;

            if ( name == "Content-Length:" )
            {
                line_buffer >> length;
                return length;
            }
        }

        return 0;
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

    static void WriteFile( string path, string contents )
    {
        ofstream out;
        out.open( path.c_str() );
        out << contents;
        out.close();
    }

    static void WriteFileToStream( int fd, string path )
    {
        ifstream file( path.c_str(), ios::in | ios::binary | ios::ate );

        if ( file.is_open() )
        {
            int size = ( int ) file.tellg();
            int block_sz = ( size < 1024 ) ? size : 1024;
            file.seekg( 0, ios::beg );

            char block[block_sz];
            int total_read = 0;

            while ( total_read < size )
            {
                bzero( block, block_sz );
                file.read( block, block_sz - 1 );
                int read_bytes = file.gcount();

                if ( read_bytes )
                {
                    write( fd, block, read_bytes );
                    total_read += read_bytes;
                }
            }

            file.close();
        }
    }
};
#endif