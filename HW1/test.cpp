/***************************************************
test.cpp

Jim Counts & Michael Ngyuen
CS 537 - Data Communications
Homework 1
**************************************************/

#include <igloo/igloo.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;
using namespace igloo;

class Server
{
private:
    int socket_fd;
    struct sockaddr_in addr;

    void acquire_socket()
    {
        socket_fd = socket( AF_INET, SOCK_STREAM, 0 );
    }

    void parse_address( string address_spec )
    {
        unsigned found = address_spec.find_first_of( ":" );

        if ( found != string::npos )
        {
            address_spec[found] = ' ';
        }

        stringstream buffer( address_spec );
        string a[2];

        for ( int i = 0; i < 2; ++i )
        {
            buffer >> a[i];
        }

        bzero( &addr, sizeof( addr ) );
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr( a[0].c_str() );
        addr.sin_port = htons( atoi( a[1].c_str() ) );
    }

public:
    Server( string address )
    {
        acquire_socket();
        parse_address( address );
    }

    ~Server()
    {
        close( socket_fd );
    }

    struct sockaddr_in get_address()
    {
        return addr;
    }

    int get_descriptor()
    {
        return socket_fd;
    }

};

int main()
{
    return TestRunner::RunAllTests();
}

Context( StandingUpAServer )
{
    Spec( AcquireAndReleaseSocketFileDescriptor )
    {
        int listenfd = -1;
        Server s( "127.0.0.1:8888" );
        listenfd = s.get_descriptor();
        Assert::That( listenfd, !Equals( -1 ) );
    }

    Spec( ConfigureAnAddress )
    {
        struct sockaddr_in servaddr;

        Server s( "127.0.0.1:8888" );
        servaddr = s.get_address();

        AssertThat( servaddr.sin_family, Equals( AF_INET ) );
        AssertThat( servaddr.sin_addr.s_addr, Equals( inet_addr( "127.0.0.1" ) ) );
        AssertThat( servaddr.sin_port, Equals( htons( 8888 ) ) ) ;
    }

    Spec( BindSocketToAddress )
    {
        Server s( "127.0.0.1:8888" );
        struct sockaddr_in servaddr = s.get_address();

        int bind_result = -1;
        bind_result = bind(
                          s.get_descriptor(),
                          ( struct sockaddr * )&servaddr,
                          sizeof( servaddr ) );
        Assert::That( bind_result, !Equals( -1 ) );
    }
};