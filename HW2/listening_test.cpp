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
#include "InternetSocket.h"
#include "TcpSocket.h"
#include "TcpListeningSocket.h"

using namespace igloo;
using namespace std;


void *Ping( void *state )
{
    struct addrinfo hints, *res;
    int sockfd;

    memset( &hints, 0, sizeof hints );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int addr_ok = getaddrinfo( "127.0.0.1", "3490", &hints, &res );

    if ( addr_ok != 0 )
    {
        fprintf( stderr,
                 "Error unable to get address info, errno = %d (%s) \n",
                 errno,
                 strerror( errno ) );
    }

    sockfd = socket( res->ai_family,
                     res->ai_socktype,
                     res->ai_protocol );

    int yes = 1;
    int opt_ok = setsockopt( sockfd,
                             SOL_SOCKET,
                             SO_REUSEADDR,
                             &yes,
                             sizeof( int ) );

    if ( opt_ok == -1 )
    {
        fprintf( stderr,
                 "Error unable to set socket options, errno = %d (%s) \n",
                 errno,
                 strerror( errno ) );
    }

    while ( connect( sockfd, res->ai_addr, res->ai_addrlen ) == -1 )
    {
        fprintf( stderr,
                 "Error unable to connect to socket, errno = %d (%s) \n",
                 errno,
                 strerror( errno ) );

        sleep( 1 );
    }

    close( sockfd );
    return NULL;
}

Context( DescribeAListeningSocket )
{
    Spec( ItCanAcceptConnections )
    {
        pthread_t tid;
        pthread_create( &tid,
                        NULL,
                        &Ping,
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

void InvalidConstruction()
{
    TcpSocket s( "512.512.512.512" );
}

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

int main( int argc, char const *argv[] )
{
    return TestRunner::RunAllTests();
}