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

using namespace igloo;
using namespace std;

void *ping( void *state );
void InvalidConstruction();

Context( DescribeAListeningSocket )
{
    Spec( ItCanAcceptConnections )
    {
        pthread_t tid;
        pthread_create( &tid,
                        NULL,
                        &ping,
                        ( void * )NULL );


        try
        {
            TcpListeningSocket t( 4,
                                  "127.0.0.1",
                                  3490 );
            SocketConnection s = t.acceptConnection();
            Assert::That( s.getDescriptor(), IsGreaterThan( -1 ) );
        }
        catch ( exception &e )
        {
            cout << e.what() << endl;
        }
    }
};

Context( DescribeAnInternetSocket )
{
    Spec( ItHasAFileDescriptor )
    {
        TcpSocket s;
        Assert::That( s.getDescriptor(), !Equals( -1 ) );
    }

    Spec( ItHasAnAddress )
    {
        TcpSocket s;
        struct sockaddr_in it = s.getAddress();
        char zero  = 0;

        for ( int i = 0; i < 8; ++i )
        {
            zero += it.sin_zero[i];
        }

        char *address = new char[INET_ADDRSTRLEN];
        inet_ntop( AF_INET,
                   &it.sin_addr.s_addr,
                   address,
                   sizeof( sockaddr_in ) );

        Assert::That( it.sin_family, Equals( AF_INET ) );
        Assert::That( ntohs( it.sin_port ), Equals( 1025 ) );
        Assert::That( address , Equals( "127.0.0.1" ) );
        Assert::That( zero, Equals( 0 ) );
    }

    Spec( TheIPAddressIsConfigurable )
    {
        TcpSocket s( "127.0.0.1" );
        struct sockaddr_in it = s.getAddress();
        char *address = new char[INET_ADDRSTRLEN];
        inet_ntop( AF_INET,
                   &it.sin_addr.s_addr,
                   address,
                   sizeof( sockaddr_in ) );
        Assert::That( address , Equals( "127.0.0.1" ) );
        delete[] address;
    }

    Spec( ThePortIsConfigurable )
    {
        TcpSocket s( "127.0.0.1", 2048 );
        struct sockaddr_in it = s.getAddress();
        Assert::That( ntohs( it.sin_port ), Equals( 2048 ) );
    }

    Spec( ItThrowsOnInvalidAddresses )
    {
        typedef InternetSocketException iaex;
        AssertThrows( iaex,
                      InvalidConstruction() );
        Assert::That( LastException<iaex>().what(),
                      Equals( "An unknown error occured" ) );
    }

};

Context( DescribeASocketException )
{
    Spec( ItPassesErrnoInformationAsAString )
    {
        InternetSocketException s( EADDRINUSE );
        Assert::That( s.what(),
                      Equals( "Address already in use" ) );
    }
};

void InvalidConstruction()
{
    TcpSocket s( "512.512.512.512" );
}

void *ping( void *state )
{
    TcpSocket ts( "127.0.0.1", 3490 );

    while ( 1 )
    {
        try
        {
            SocketConnection sc = ts.initiateConnection();
            break;
        }
        catch ( exception &e )
        {
            cout << e.what() << endl;
        }

        sleep( 1 );
    }

    return NULL;
}

int main( int argc, char const *argv[] )
{
    return TestRunner::RunAllTests();
}