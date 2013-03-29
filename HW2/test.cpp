/*test.cpp*/

#include <arpa/inet.h>
#include <igloo/igloo.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
// #include <sys/types.h>
#include "TcpSocket.h"
// #include "SocketConnection.h"

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
    struct addrinfo hints;
    memset( &hints, 0, sizeof hints );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    int rv;
    struct addrinfo  *servinfo;

    if ( ( rv = getaddrinfo( NULL, "3490", &hints, &servinfo ) ) != 0 )
    {
        fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( rv ) );
        return NULL;
    }

    // loop through all the results and bind to the first we can
    struct addrinfo *p;

    int sockfd;  // listen on sock_fd, new connection on new_fd

    for ( p = servinfo; p != NULL; p = p->ai_next )
    {
        if ( ( sockfd = socket( p->ai_family,
                                p->ai_socktype,
                                p->ai_protocol ) ) == -1 )
        {
            perror( "server: socket" );
            continue;
        }

        int yes = 1;

        if ( setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                         sizeof( int ) ) == -1 )
        {
            perror( "setsockopt" );
            exit( 1 );
        }

        if ( bind( sockfd, p->ai_addr, p->ai_addrlen ) == -1 )
        {
            close( sockfd );
            perror( "server: bind" );
            continue;
        }

        break;
    }

    if ( p == NULL )
    {
        fprintf( stderr, "server: failed to bind\n" );
        return NULL;
    }

    freeaddrinfo( servinfo ); // all done with this structure

    if ( listen( sockfd, 4 ) == -1 )
    {
        perror( "listen" );
        exit( 1 );
    }

    for ( int i = 0; i < 10; ++i )
    {
        struct sockaddr_storage their_addr; // connector's address information
        socklen_t sin_size = sizeof their_addr;
        int new_fd = accept( sockfd,
                             ( struct sockaddr * )&their_addr,
                             &sin_size );

        if ( new_fd == -1 )
        {
            perror( "accept" );
            return NULL;
        }

        char str[1024];
        bzero( str, 1023 );

        if ( read( new_fd, str, 1023 )  == 0 )
        {
            close( new_fd );
            return NULL;
        }

        for ( unsigned int i = 0; i < strlen( str ); i++ )
        {
            if ( str[i] >= 97 && str[i] <= 122 )
            {
                str[i] = str[i] - 32;
            }
        }

        write( new_fd, str, strlen( str ) );

        close( new_fd );
    }

    close( sockfd );
    return NULL;
}

void *pong( void *state )
{
    struct addrinfo hints;
    memset( &hints, 0, sizeof hints );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    int rv;
    struct addrinfo  *servinfo;

    if ( ( rv = getaddrinfo( NULL, "3490", &hints, &servinfo ) ) != 0 )
    {
        fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( rv ) );
        return NULL;
    }

    // loop through all the results and bind to the first we can
    struct addrinfo *p;

    int sockfd;  // listen on sock_fd, new connection on new_fd

    for ( p = servinfo; p != NULL; p = p->ai_next )
    {
        if ( ( sockfd = socket( p->ai_family,
                                p->ai_socktype,
                                p->ai_protocol ) ) == -1 )
        {
            perror( "server: socket" );
            continue;
        }

        int yes = 1;

        if ( setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                         sizeof( int ) ) == -1 )
        {
            perror( "setsockopt" );
            exit( 1 );
        }

        if ( bind( sockfd, p->ai_addr, p->ai_addrlen ) == -1 )
        {
            close( sockfd );
            perror( "server: bind" );
            continue;
        }

        break;
    }

    if ( p == NULL )
    {
        fprintf( stderr, "server: failed to bind\n" );
        return NULL;
    }

    freeaddrinfo( servinfo ); // all done with this structure

    if ( listen( sockfd, 4 ) == -1 )
    {
        perror( "listen" );
        exit( 1 );
    }

    struct sockaddr_storage their_addr; // connector's address information

    socklen_t sin_size = sizeof their_addr;

    int new_fd = accept( sockfd,
                         ( struct sockaddr * )&their_addr,
                         &sin_size );

    if ( new_fd == -1 )
    {
        perror( "accept" );
        return NULL;
    }

    close( sockfd ); // child doesn't need the listener

    if ( send( new_fd, "Hello, world!", 13, 0 ) == -1 )
    {
        perror( "send" );
    }

    close( new_fd );

    return NULL;
}

int main( int argc, char const *argv[] )
{
    return TestRunner::RunAllTests();
}