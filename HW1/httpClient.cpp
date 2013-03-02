#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXLINE 256

using namespace std;

void http_client( FILE *fp, int socket_fd );

int main( int argc, char *argv[] )
{

    int	socket_fd;
    struct  sockaddr_in servaddr;

    if ( argc != 3 )
    {
        printf( "Usage: httpClient <address> <port> \n" );
        return -1;
    }

    socket_fd = socket( AF_INET, SOCK_STREAM, 0 );

    if ( socket_fd < 0 )
    {
        fprintf( stderr, "Error creating socket, errno = %d (%s) \n",
                 errno, strerror( errno ) );
        return -1;
    }

    bzero( &servaddr, sizeof( servaddr ) );
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr( argv[1] );
    servaddr.sin_port = htons( atoi( argv[2] ) );

    if ( connect( socket_fd, ( struct sockaddr * )&servaddr, sizeof( servaddr ) ) == -1 )
    {
        fprintf( stderr, "Error unable to connect to socket, errno = %d (%s) \n", errno,
                 strerror( errno ) );
        return -1;
    }

    http_client( stdin, socket_fd );

    return 0;

}

void http_client( FILE *fp, int socket_fd )
{
    int buffer_size = 1024;
    char buffer[buffer_size];

    string request( "HEAD /test.html HTTP/1.0\n\n" );

    write( socket_fd, ( void * )request.c_str(), strlen( request.c_str() ) );

    int last_read = 0;

    string result;

    do
    {
        bzero( buffer, buffer_size );
        last_read = read(
                        socket_fd,
                        ( void * )buffer,
                        buffer_size );

        if ( last_read == -1 )
        {
            break;
        }


        result += buffer;
    }
    while ( 0 < last_read );

    cout << result << endl;
}


