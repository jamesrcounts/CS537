#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#include "HttpRequest.h"

#define MAXLINE	32768

using namespace std;

const int backlog = 4;

void *clientHandler( void *arg )
{
    int fd = *( int * )( arg );
    HttpRequest::Handle( fd );
}

int main( int argc, char *argv[] )
{
    if ( argc != 3 )
    {
        printf( "Usage: httpServer <address> <port> \n" );
        return -1;
    }

    int listenfd = socket( AF_INET, SOCK_STREAM, 0 );

    if ( listenfd == -1 )
    {
        fprintf( stderr,
                 "Error unable to create socket, errno = %d (%s) \n",
                 errno,
                 strerror( errno ) );
        return -1;
    }

    struct  sockaddr_in servaddr;

    bzero( &servaddr, sizeof( servaddr ) );

    servaddr.sin_family 	   = AF_INET;

    servaddr.sin_addr.s_addr   = inet_addr( argv[1] );

    servaddr.sin_port          = htons( atoi( argv[2] ) );

    int bind_result = bind( listenfd,
                            ( struct sockaddr * )&servaddr,
                            sizeof( servaddr ) );

    if ( bind_result == -1 )
    {
        fprintf( stderr,
                 "Error binding to socket, errno = %d (%s) \n",
                 errno,
                 strerror( errno ) );
        return -1;

    }

    if ( listen( listenfd, backlog ) == -1 )
    {
        fprintf( stderr,
                 "Error listening for connection request, errno = %d (%s) \n",
                 errno,
                 strerror( errno ) );
        return -1;
    }

    while ( 1 )
    {
        struct  sockaddr_in cliaddr;
        int clilen = sizeof( cliaddr );
        int connfd = accept( listenfd,
                             ( struct sockaddr * )&cliaddr,
                             ( socklen_t * )&clilen );

        if ( connfd < 0 )
        {
            if ( errno == EINTR )
            {
                continue;
            }
            else
            {
                fprintf( stderr,
                         "Error connection request refused, errno = %d (%s) \n",
                         errno,
                         strerror( errno ) );
            }
        }

        pthread_t tid;
        int create_result = pthread_create( &tid,
                                            NULL,
                                            clientHandler,
                                            ( void * )&connfd );

        if ( create_result != 0 )
        {
            fprintf( stderr,
                     "Error unable to create thread, errno = %d (%s) \n",
                     errno,
                     strerror( errno ) );
        }

    }
}
