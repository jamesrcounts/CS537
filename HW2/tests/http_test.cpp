/* test.cpp */

#include <igloo/igloo.h>
#include <approvals/Approvals.h>
#include <arpa/inet.h>
#include <errno.h>
#include <algorithm>
#include <iterator>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include "../InternetSocket.h"
#include "../TcpSocket.h"
#include "../TcpListeningSocket.h"

using namespace igloo;
using namespace std;

void *server( void *state );

Context( DescribeAWebServer )
{
    void startServer( pthread_t * tid )
    {
        for ( int i = 0; i < 10; ++i )
        {
            try
            {
                pthread_create( tid,
                                NULL,
                                &server,
                                ( void * )NULL );
                break;
            }
            catch ( InternetSocketException &e )
            {
                cerr << e.what() << endl;
            }

            sleep( 1 );
        }
    }

    string sendRequest( string request )
    {
        string response;

        for ( int i = 0; i < 10; ++i )
        {
            sleep( 1 );

            try
            {
                TcpSocket ts( "127.0.0.1", 3490 );
                SocketConnection s = ts.initiateConnection();
                s.sendMessage( request );
                response = s.readMessage();
                break;
            }
            catch ( InternetSocketException &e )
            {
                cout << e.what() << endl;
            }
        }

        return response;
    }

    Spec( ItCanHandlePutRequests )
    {
        pthread_t tid;
        startServer( &tid );
        string response = sendRequest( "PUT /greeting.txt HTTP/1.0\n\nHello" );
        pthread_join( tid, NULL );
        Approvals::Verify( response );
        AssertThat( FileApprover::FileExists( IglooNamerFactory::TestDirectory() + "/pub" + "/greeting.txt" ),
                    Equals( true ) );
    }

    Spec( ItCanHandleGetRequests )
    {
        pthread_t tid;
        startServer( &tid );
        string response = sendRequest( "GET /test.html HTTP/1.0\n\n" );
        pthread_join( tid, NULL );
        Approvals::Verify( response );
    }

    Spec( ItNotifiesClientWhenResourceNotFound )
    {
        pthread_t tid;
        startServer( &tid );
        string response = sendRequest( "HEAD /noexist.html HTTP/1.0\n\n" );
        pthread_join( tid, NULL );
        Approvals::Verify( response );
    }

    Spec( ItRespondsToUnknownCommands )
    {
        pthread_t tid;
        startServer( &tid );
        string response = sendRequest( "FOO /test.html HTTP/1.0\n\n" );
        pthread_join( tid, NULL );
        Approvals::Verify( response );
    }

    Spec( ItCanHandleHeadRequests )
    {
        pthread_t tid;
        startServer( &tid );
        string response = sendRequest( "HEAD /test.html HTTP/1.0\n\n" );
        pthread_join( tid, NULL );
        Approvals::Verify( response );
    }
};

string FixupPath( string resource )
{
    char buf[512];
    bzero( buf, 512 );
    ssize_t readlink_ok = readlink( "/proc/self/exe", buf, 511 );

    string dir( "." );

    if ( readlink_ok != -1 )
    {
        dir = string( buf );
    }

    unsigned slash = dir.find_last_of( "/" );

    if ( slash != std::string::npos )
    {
        dir = dir.substr( 0, slash );
    }

    string path = dir + "/pub" + resource;
    return path;
}

void *server( void *state )
{
    TcpListeningSocket t( 4, "127.0.0.1", 3490 );
    SocketConnection s = t.acceptConnection();
    string str = s.readMessage();

    stringstream requestStream( str );
    string keys[] = {"COMMAND", "RESOURCE", "VERSION"};
    std::map<string, string> request;

    for ( int i = 0; i < 3; ++i )
    {
        requestStream >> request[keys[i]];
    }

    stringstream responseStream;

    if ( "HEAD" == request["COMMAND"] || "GET" == request["COMMAND"] )
    {
        string path = FixupPath( request["RESOURCE"] );

        struct stat statbuf;

        if ( stat( path.c_str(), &statbuf ) != -1 )
        {
            int buffer_sz = 80;
            char mod_time[buffer_sz];
            bzero( mod_time, buffer_sz );
            struct tm *timeinfo = localtime( &statbuf.st_mtime );
            strftime( mod_time, buffer_sz - 1, "%c", timeinfo );

            responseStream << request["VERSION"] << " 200 OK\n" <<
                           "Server: myhttpd/1.0\n" <<
                           "Last-Modified: " << mod_time << "\n" <<
                           "Content-Length: " << statbuf.st_size << "\n" <<
                           "\n";

            if ( "GET" == request["COMMAND"] )
            {
                ifstream file( path.c_str(), ios::in | ios::binary | ios::ate );

                if ( file.is_open() )
                {
                    int size( file.tellg() );
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
                            responseStream << block;
                            total_read += read_bytes;
                        }
                    }

                    file.close();
                }
            }

        }
        else
        {
            responseStream << request["VERSION"] << " 404 Not Found\n";
        }
    }
    else if ( "PUT" == request["COMMAND"] )
    {
        string path = FixupPath( request["RESOURCE"] );
        ofstream out;
        out.open( path.c_str() );
        std::istreambuf_iterator<char> eos;
        std::string s( std::istreambuf_iterator<char>( requestStream ), eos );
        out << s;
        out.close();

        // if ( version == 1.1f )
        // {
        //     WriteToStream( fd, string( "HTTP/1.1 100 Continue" ) );
        // }

        // string response = HttpHandler::StatusOk( tokens[2] );

        // if ( length != 0 )
        // {
        // }
        // else
        // {
        //     response = HttpHandler::StatusBadRequest( tokens[2] );
        // }

        // WriteToStream( fd, response + HttpHandler::BlankLine() );
    }

    else
    {
        cerr << "Recieved " <<
             request["COMMAND"] <<
             ", which this server does not support." << endl;
        responseStream << request["VERSION"] << " 501 Not Implemented\n";
    }

    s.sendMessage( responseStream.str() );
    return NULL;
}

int main( int argc, char const *argv[] )
{
    DefaultTestResultsOutput output;
    TestRunner runner( output );

    IglooMetadataListener listener;
    runner.AddListener( &listener );
    MinimalProgressTestListener mpl;
    runner.AddListener( &mpl );

    return runner.Run();
}