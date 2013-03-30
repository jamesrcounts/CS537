/* test.cpp */

#include <igloo/igloo.h>

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include "../InternetSocket.h"
#include "../TcpSocket.h"
#include "../TcpListeningSocket.h"
#include "AddApprovalMetadata.h"

using namespace igloo;
using namespace std;

void *server( void *state );

Context( DescribeAWebServer )
{
    Spec( ItCanHandleGetRequests )
    {
        cout << Name() << ": " << GetAttribute( "SpecName" ) << endl;

        pthread_t tid;
        pthread_create( &tid,
                        NULL,
                        &server,
                        ( void * )NULL );

        for ( int i = 0; i < 10; ++i )
        {
            sleep( 1 );

            try
            {
                TcpSocket ts( "127.0.0.1", 3490 );
                SocketConnection s = ts.initiateConnection();
                s.sendMessage( "ok" );
                string response = s.readMessage();
                Assert::That( response, Equals( "OK" ) );
                break;
            }
            catch ( InternetSocketException &e )
            {
                cout << e.what() << endl;
            }

        }
    }
};

void *server( void *state )
{
    TcpListeningSocket t( 4, "127.0.0.1", 3490 );
    SocketConnection s = t.acceptConnection();
    string str = s.readMessage();
    transform( str.begin(), str.end(), str.begin(), ::toupper );
    s.sendMessage( str );
    return NULL;
}

int main( int argc, char const *argv[] )
{
    DefaultTestResultsOutput output;
    TestRunner runner( output );

    AddApprovalMetadata listener;
    runner.AddListener( &listener );
    MinimalProgressTestListener mpl;
    runner.AddListener( &mpl );

    return runner.Run();
    // return TestRunner::RunAllTests();
}