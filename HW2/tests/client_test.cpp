/*test.cpp*/

#include <arpa/inet.h>
#include <igloo/igloo.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../TcpSocket.h"
#include "../TcpListeningSocket.h"

using namespace igloo;

void *caseServer( void *state );
void *pong( void *state );

Context( DescribeAClientSocket )
{
    Spec( ItHasAnAddressItWantsToConnectTo )
    {
        TcpSocket is( "127.0.0.1", 3490 );
        struct sockaddr_in it = is.getAddress();
        char *address = new char[INET_ADDRSTRLEN];
        inet_ntop( AF_INET,
                   &it.sin_addr.s_addr,
                   address,
                   sizeof( sockaddr_in ) );
        Assert::That( address , Equals( "127.0.0.1" ) );
        delete[] address;
        Assert::That( ntohs( it.sin_port ), Equals( 3490 ) );
    }

    Spec( ItHasAFileDescriptor )
    {
        TcpSocket ts( "127.0.0.1", 3490 );
        Assert::That( ts.getDescriptor(), IsGreaterThan( -1 ) );
    }

    Spec( ItThrowsAnExceptionWhenItCantConnect )
    {
        TcpSocket ts( "127.0.0.1", 80 );
        AssertThrows( InternetSocketException, ts.initiateConnection() );
        Assert::That( LastException<InternetSocketException>().what(),
                      Equals( "Connection refused" ) );

    }

    Spec( ItCanConnectToListeningSockets )
    {
        try
        {
            TcpSocket ts( "144.37.12.45",
                          80 );
            SocketConnection s = ts.initiateConnection();
            Assert::That( s.getDescriptor(), IsGreaterThan( -1 ) );
        }
        catch ( exception &e )
        {
            cout << e.what() << endl;
        }
    }
};

Context( DescribeASocketConnection )
{
    Spec( ItCanRecieveAMessage )
    {
        pthread_t tid;
        pthread_create( &tid,
                        NULL,
                        &pong,
                        ( void * )NULL );

        for ( int i = 0; i < 10; ++i )
        {
            sleep( 1 );

            try
            {
                TcpSocket ts( "127.0.0.1", 3490 );
                SocketConnection s = ts.initiateConnection();
                string response = s.readMessage();
                Assert::That( response, Equals( "Hello, world!" ) );
                break;
            }
            catch ( InternetSocketException &e )
            {
                cout << e.what() << endl;
            }

        }
    }

    Spec( ItCanSendAMessage )
    {
        pthread_t  tid;
        pthread_create( &tid,
                        NULL,
                        &caseServer,
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

void *caseServer( void *state )
{
    TcpListeningSocket t( 4, "127.0.0.1", 3490 );
    SocketConnection s = t.acceptConnection();
    string str = s.readMessage();
    transform( str.begin(), str.end(), str.begin(), ::toupper );
    s.sendMessage( str );
    return NULL;
}

void *pong( void *state )
{
    TcpListeningSocket t( 4, "127.0.0.1", 3490 );
    SocketConnection s = t.acceptConnection();
    s.sendMessage( "Hello, world!" );
    return NULL;
}

int main( int argc, char const *argv[] )
{
    return TestRunner::RunAllTests();
}