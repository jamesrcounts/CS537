#ifndef SOCKET_H
#define SOCKET_H

#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <string.h>
#include <strings.h>
#include <unistd.h>

using namespace std;

class Socket
{
private:
    struct sockaddr_in addr;

    int socket_fd;

protected:
    void acquire_socket( int options = 0 )
    {
        socket_fd = socket( AF_INET, SOCK_STREAM | options, 0 );
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

    static string read_stream( int fd )
    {
        int buffer_size = 1024;
        char buffer[buffer_size];
        int result_size = 0;
        char *result = new char[result_size];

        int last_read = 0;
        int read_err = 0;

        // States:
        // havent read anything/ got EAGAIN
        // last_read == 0 && read_err == EAGAIN
        // read something
        // last_read > 0 && read_err == ??
        // read something / got eagain.
        // result_size > 0 &&  read_err == EAGAIN

        do
        {
            last_read = read(
                            fd,
                            ( void * )buffer,
                            buffer_size );
            read_err = errno;
            cout <<
                 "Error unable to read socket, errno = " <<
                 read_err << " " << strerror( read_err ) << endl;

            cout << "EAGAIN would be: " << EAGAIN << endl;

            cout << "Last read: " << last_read << endl;

            if ( read_err == EAGAIN )
            {
                continue;
            }

            if ( last_read == -1 )
            {
                break;
            }

            char *t = result;
            result = new char[result_size + last_read];
            memcpy( result, t, result_size );
            memcpy( result + result_size, buffer, last_read );
            result_size += last_read;

            cout << result << endl;

            delete[] t;
        }
        while ( read_err == EAGAIN || result_size == 0 );

        string contents( result );
        delete[] result;
        return contents;
    }

    static int write_stream( int fd, string contents )
    {
        return  write( fd, contents.c_str(), contents.size() );
    }

};

#endif