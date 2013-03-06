#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <iostream>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXLINE 32768

using namespace std;

float version = 1.0f;
int master_socket = -1;

int acquire_socket( in_addr_t s_addr, uint16_t sin_port )
{
    if ( master_socket == -1 )
    {
        int socket_fd = socket( AF_INET, SOCK_STREAM, 0 );

        if ( socket_fd < 0 )
        {
            fprintf( stderr,
                     "Error creating socket, errno = %d (%s) \n",
                     errno,
                     strerror( errno ) );
            return -1;
        }

        struct  sockaddr_in servaddr;

        bzero( &servaddr, sizeof( servaddr ) );

        servaddr.sin_family = AF_INET;

        servaddr.sin_addr.s_addr = s_addr;

        servaddr.sin_port = sin_port;

        int connect_result =  connect( socket_fd,
                                       ( struct sockaddr * )&servaddr,
                                       sizeof( servaddr ) );

        if ( connect_result == -1 )
        {
            fprintf( stderr,
                     "Error unable to connect to socket, errno = %d (%s) \n",
                     errno,
                     strerror( errno ) );
            return -1;
        }

        master_socket = socket_fd;
    }

    return master_socket;
}

void release_socket()
{
    if ( version == 1.0f )
    {
        close( master_socket );
        master_socket = -1;
    }
}

void send_request( int socket_fd )
{
    string request( "GET /test.html HTTP/1.0\n\n" );
    write( socket_fd, request.c_str(), request.size() );
}

string get_response( int socket_fd )
{
    int buffer_size = MAXLINE;
    char buffer[MAXLINE + 1];
    bzero( buffer, buffer_size );
    int last_read = read( socket_fd, buffer, buffer_size );

    if ( -1 == last_read )
    {
        fprintf( stderr,
                 "Error unable to read socket, errno = %d (%s) \n",
                 errno,
                 strerror( errno ) );
        exit( -1 );
    }

    return string( buffer );
}

int main( int argc, char const *argv[] )
{
    if ( argc < 3 )
    {
        printf( "Usage: httpClient <address> <port> [version]\n" );
        return -1;
    }

    if ( argc == 4 )
    {
        version = strtof( argv[3], NULL );
        cout << "Using HTTP " << version << " as requested..." << endl;
    }


    int i = 0;
    struct timeval startat, endat;
    gettimeofday( &startat, NULL );

    for ( ; i < ( 1000 ); ++i )
    {
        int socket_fd = acquire_socket(
                            inet_addr( argv[1] ),
                            htons( atoi( argv[2] ) ) );

        send_request( socket_fd );
        get_response( socket_fd );

        release_socket();
    }

    gettimeofday( &endat, NULL );

    int sec = endat.tv_sec - startat.tv_sec;
    int usec = endat.tv_usec - startat.tv_usec;

    if ( usec < 0 )
    {
        --sec;
        usec += 1000000;
    }

    cout << i << " requests in: "
         << sec << " seconds and "
         << usec << " microseconds." << endl;

    return 0;
}
