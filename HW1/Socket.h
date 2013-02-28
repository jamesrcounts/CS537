#ifndef SOCKET_H
#define SOCKET_H

#include <arpa/inet.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <strings.h>
#include <unistd.h>

using namespace std;

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

#endif