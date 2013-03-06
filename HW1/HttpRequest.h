#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <iostream>
#include <string>
#include <vector>
#include "HttpHandler.h"

#define MAXREAD 32768

using namespace std;

class HttpRequest
{
public:
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
    static void Handle( float version, int fd, string request )
    {
        vector<string> tokens = ParseRequest( request );
        string header = ReadHeader( fd );

        if ( tokens[0] == "GET" )
        {
            WriteToStream( fd,
                           HttpHandler::Head( tokens[2], tokens[1] ) );

            string path = HttpHandler::FixupPath( tokens[1] );

            if ( HttpHandler::FileExists( path ) )
            {
                WriteFileToStream( fd, path );
            }
        }
        else if ( tokens[0] == "HEAD" )
        {
            WriteToStream( fd,
                           HttpHandler::Head( tokens[2], tokens[1] ) );
        }
        else if ( tokens[0] == "PUT" )
        {
            if ( version == 1.1f )
            {
                WriteToStream( fd, string( "HTTP/1.1 100 Continue" ) );
            }

            string response = HttpHandler::StatusOk( tokens[2] );

            int length = HttpHandler::StreamLength( header );

            if ( length != 0 )
            {
                HttpHandler::WriteFile(
                    HttpHandler::FixupPath( tokens[1] ),
                    HttpHandler::ReadStream( fd, length ) );
            }
            else
            {
                response = HttpHandler::StatusBadRequest( tokens[2] );
            }

            WriteToStream( fd, response + HttpHandler::BlankLine() );
        }
        else if ( tokens[0] == "DELETE" )
        {
            WriteToStream( fd,
                           HttpHandler::Delete( tokens[2], tokens[1] ) );
        }
        else
        {
            cout << "Got " << tokens[0] << " which I havent implemented." << endl;
            WriteToStream( fd,
                           HttpHandler::StatusNotImplemented( tokens[2] ) );
        }


    }

    static vector<string> ParseRequest( string header )
    {
        vector<string> tokens;
        stringstream header_stream( header );

        for ( int i = 0; i < 3; ++i )
        {
            string t;
            header_stream >> t ;
            tokens.push_back( t );
        }

        return tokens;
    }

    static string ReadHeader( int fd )
    {
        char buffer[MAXREAD];
        bzero( buffer, MAXREAD );

        read( fd, buffer, MAXREAD - 1 );
        return string( buffer );
    }

    static void WriteToStream( int fd, string message )
    {
        write( fd, message.c_str(), message.size() );
    }
};
#endif