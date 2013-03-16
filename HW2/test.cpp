/* test.cpp */

#include <igloo/igloo.h>

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include "InternetSocket.h"

using namespace igloo;
using namespace std;

class TcpSocket : public InternetSocket
{
public:
    TcpSocket( string address_spec = "", uint16_t port = 1025 )
        : InternetSocket( address_spec, port )
    {
        fd = socket( AF_INET, SOCK_STREAM, 0 );

        if ( fd < 0 )
        {
            int error = errno;
            throw SocketException( error );
        }

        int yes = 1;
        int opt_ok = setsockopt( fd,
                                 SOL_SOCKET,
                                 SO_REUSEADDR,
                                 &yes,
                                 sizeof( int ) );

        if ( opt_ok == -1 )
        {
            int error = errno;
            throw InternetSocketException( error );
        }

        int bind_ok = bind( fd,
                            ( struct sockaddr * )&address,
                            sizeof( address ) );

        if ( bind_ok == -1 )
        {
            int error = errno;
            throw InternetSocketException( error );
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
        typedef InvalidAddressException iaex;
        AssertThrows( iaex,
                      InvalidConstruction() );
        Assert::That( LastException<iaex>().what(),
                      Equals( "Tried to create a socket with an invalid address" ) );
    }

};

Context( DescribeASocketException )
{
    Spec( ItPassesErrnoInformationAsAString )
    {
        SocketException s( EADDRINUSE );
        Assert::That( s.what(),
                      Equals( "Could not create socket: Address already in use" ) );
    }
};

int main( int argc, char const *argv[] )
{
    return TestRunner::RunAllTests();
}