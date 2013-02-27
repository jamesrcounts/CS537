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
    int bind_result;
    int listen_result;
    int queue_size;
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
        : socket_fd( -1 ), bind_result( -1 ), listen_result( -1 ), queue_size( 4 )
    {
        parse_address( address );
        acquire_socket();
        bind_address();
        socket_listen();
    }

    ~Server()
    {
        close( socket_fd );
    }

    void bind_address()
    {
        if ( !is_bound() )
        {
            bind_result = bind(
                              socket_fd,
                              ( struct sockaddr * )&addr,
                              sizeof( addr ) );
        }
    }

    struct sockaddr_in get_address()
    {
        return addr;
    }

    int get_descriptor()
    {
        return socket_fd;
    }

    bool is_bound()
    {
        return bind_result != -1;
    }

    bool is_listening()
    {
        return listen_result != -1;
    }

    void socket_listen()
    {
        if ( !is_listening() )
        {
            listen_result = listen( socket_fd, queue_size );
        }
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
        Assert::That( server->get_descriptor(), !Equals( -1 ) );
    }

    Spec( ConfigureAnAddress )
    {
        struct sockaddr_in servaddr;
        servaddr = server->get_address();
        AssertThat( servaddr.sin_family, Equals( AF_INET ) );
        AssertThat(
            servaddr.sin_addr.s_addr,
            Equals( inet_addr( "127.0.0.1" ) ) );
        AssertThat( servaddr.sin_port, Equals( htons( 8888 ) ) ) ;
    }

    Spec( BindSocketToAddress )
    {
        Assert::That( server->is_bound(), Equals( true ) );
    }

    Spec( ListenForIncomingConnections )
    {
        Assert::That( server->is_listening(), Equals( true ) );
    }

    Server *server;

    // cppcheck-suppress unusedFunction
    void SetUp()
    {
        server = new Server( "127.0.0.1:8888" );
    }

    // cppcheck-suppress unusedFunction
    void TearDown()
    {
        delete server;
    }
};