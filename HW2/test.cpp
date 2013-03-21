/*test.cpp*/

#include <arpa/inet.h>
#include <igloo/igloo.h>
#include "TcpSocket.h"
#include "SocketConnection.h"

using namespace igloo;

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

int main( int argc, char const *argv[] )
{
    return TestRunner::RunAllTests();
}