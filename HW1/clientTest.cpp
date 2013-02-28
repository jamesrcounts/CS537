#include <igloo/igloo.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#include "Client.h"
#include "Server.h"
#include "Socket.h"

using namespace std;
using namespace igloo;


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
