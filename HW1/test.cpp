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

class Socket
{
private:
    struct sockaddr_in addr;

    int socket_fd;

protected:
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
    Socket() : socket_fd( -1 ) {}

    ~Socket()
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

class Server : public Socket
{
private:
    int bind_result;
    int listen_result;
    int queue_size;

public:
    Server( string address )
        :  bind_result( -1 ), listen_result( -1 ), queue_size( 4 )
    {
        parse_address( address );
        acquire_socket();
        bind_address();
        socket_listen();
    }

    ~Server()
    {
    }

    void bind_address()
    {
        if ( !is_bound() )
        {
            struct sockaddr_in address = get_address();
            bind_result = bind(
                              get_descriptor(),
                              ( struct sockaddr * )&address,
                              sizeof( address ) );
        }
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
            listen_result = listen(
                                get_descriptor(),
                                queue_size );
        }
    }
};

class Client : public Socket
{
private:
    int connection_result;
    int written_bytes;
    int read_bytes;

    string server_response;
public:
    Client( string address )
        : connection_result( -1 ), written_bytes( 0 ), read_bytes( 0 )
    {
        parse_address( address );
        acquire_socket();
    }

    ~Client() {}

    int last_received()
    {
        return read_bytes;
    }

    string last_response()
    {
        return server_response;
    }

    int last_send()
    {
        return written_bytes;
    }

    void receive()
    {
        int buffer_size = 1024;
        char buffer[buffer_size];
        int result_size = 0;
        char *result = new char[result_size];

        int last_read = 0;

        do
        {
            last_read = read(
                            get_descriptor(),
                            ( void * )buffer,
                            buffer_size );

            if ( last_read == -1 )
            {
                break;
            }

            char *t = result;
            result = new char[result_size + last_read];
            memcpy( result, t, result_size );
            memcpy( result + result_size, buffer, last_read );
            result_size += last_read;

            delete[] t;
        }
        while ( 0 < last_read );

        read_bytes = result_size;
        server_response = string( result );
        delete[] result;
    }

    void send( string request )
    {
        written_bytes = write(
                            get_descriptor(),
                            ( void * )request.c_str(),
                            request.size() );
    }


    void socket_connect()
    {
        if ( !is_connected() )
        {
            struct sockaddr_in address = get_address();
            connection_result = connect(
                                    get_descriptor(),
                                    ( struct sockaddr * )&address,
                                    sizeof( address ) );
        }
    }

    bool is_connected()
    {
        return connection_result == 0;
    }
};

int main()
{
    return TestRunner::RunAllTests();
}

Context( MakingAClientRequest )
{
    Spec( AcquireASocket )
    {
        Assert::That( client->get_descriptor(),
                      IsGreaterThan( -1 ) );
    }

    Spec( ConfigureAnAddress )
    {
        struct sockaddr_in address;
        address = client->get_address();
        AssertThat( address.sin_family, Equals( AF_INET ) );
        AssertThat(
            address.sin_addr.s_addr,
            Equals( inet_addr( "144.37.12.45" ) ) );
        AssertThat( address.sin_port, Equals( htons( 80 ) ) );
    }

    Spec( ConnectToRemote )
    {
        Assert::That( client->is_connected(), Equals( true ) );
    }

    Spec( SendARequest )
    {
        Assert::That(
            client->last_send(),
            Equals( ( int )request.size() ) );
    }

    Spec( GetResult )
    {
        client->receive();

        int response_bytes = client->last_received();
        AssertThat( response_bytes, IsGreaterThan( 15 ) );

        string response = client->last_response();
        string status = response.substr( 0, 15 );
        AssertThat( status, Equals( "HTTP/1.1 200 OK" ) );
    }

    Client *client;
    string request;

    // cppcheck-suppress unusedFunction
    void SetUp()
    {
        request = "GET / HTTP/1.0\n\n";
        client = new Client( "144.37.12.45:80" );
        client->socket_connect();
        client->send( request );
    }

    // cppcheck-suppress unusedFunction
    void TearDown()
    {
        delete client;
    }
};

Context( StandingUpAServer )
{
    Spec( AcquireAndReleaseSocketFileDescriptor )
    {
        Assert::That( server->get_descriptor(), IsGreaterThan( -1 ) );
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