#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <string>
#include <vector>
#include "HttpHandler.h"

#define MAXREAD 32768

using namespace std;

class HttpRequest
{
public:
    HttpRequest();
    ~HttpRequest();

    static void Delete( int fd, vector<string> tokens )
    {
        WriteToStream( fd, HttpHandler::Delete( tokens[2], tokens[1] ) );
        close( fd );
    }

    static void Get( int fd, vector<string> tokens )
    {
        HttpHandler::Get( fd, tokens[2], tokens[1] );
        close( fd );
    }

    static void Handle( int fd )
    {
        string request = ReadRequest( fd );
        string header = ReadHeader( fd );

        vector<string> tokens = ParseRequest( request );

        if ( tokens[0] == "GET" )
        {
            Get( fd, tokens );
        }
        else if ( tokens[0] == "HEAD" )
        {
            Head( fd, tokens );
        }
        else if ( tokens[0] == "PUT" )
        {
            Put( fd, tokens, header );
        }
        else if ( tokens[0] == "DELETE" )
        {
            Delete( fd, tokens );
        }
        else
        {
            NotImplemented( fd, tokens );
        }

    }

    static void Head( int fd, vector<string> tokens )
    {
        WriteToStream( fd, HttpHandler::Head( tokens[2], tokens[1] ) );
        close( fd );
    }

    static void NotImplemented( int fd, vector<string> request )
    {
        WriteToStream( fd, HttpHandler::StatusNotImplemented( request[2] ) );
        close( fd );
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

    static void Put( int fd, vector<string> tokens, string header )
    {
        WriteToStream( fd,
                       HttpHandler::Put( tokens[2], header, tokens[1], fd ) );
        close( fd );
    }

    static string ReadHeader( int fd )
    {
        char buffer[MAXREAD];
        bzero( buffer, MAXREAD );

        read( fd, buffer, MAXREAD - 1 );
        return string( buffer );
    }

    static string ReadRequest( int fd )
    {
        size_t idx = 0;
        size_t buffer_sz = 1024;
        char buffer[buffer_sz];
        bzero( buffer, buffer_sz );

        while ( idx < ( buffer_sz - 1 ) && 1 == read( fd, &buffer[idx], 1 ) )
        {
            if ( buffer[idx] == '\n' )
            {
                break;
            }

            ++idx;
        }

        return string( buffer );
    }

    static void WriteToStream( int fd, string message )
    {
        write( fd, message.c_str(), message.size() );
    }
};
#endif