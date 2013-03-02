/***************************************************
Jim Counts & Michael Ngyuen
CS 537 - Data Communications
Homework 1
**************************************************/

#include <igloo/igloo.h>

#include "Server.h"

using namespace std;
using namespace igloo;

int main()
{
    return TestRunner::RunAllTests();
}

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