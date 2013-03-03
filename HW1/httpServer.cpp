#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <sstream>
#include <time.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "HttpHandler.h"
#include "HttpRequest.h"

#define MAXLINE	32768
// #define MAXLINE  128

using namespace std;

const int backlog = 4;

void httpDelete( string header, int fd )
{
    HttpRequest::Delete( fd, header );
}

void httpPut( string request, string full_request, int fd )
{
    HttpRequest::Put( fd, request, full_request );
}

void httpHead( string request, int fd )
{
    HttpRequest::Head( fd, request );
}

void httpGet( string request, int fd )
{
    HttpRequest::Get( fd, request );
}

void *clientHandler( void *arg )
{
    int fd = *( int * )( arg );

    string request = HttpRequest::ReadRequest( fd );
    cout << "Request: " << endl  << request << endl;
    stringstream request_stream( request );
    string command;
    request_stream >> command;

    string header = HttpRequest::ReadHeader( fd );

    if ( command == "GET" )
    {
        httpGet( request, fd );
    }
    else if ( command == "HEAD" )
    {
        httpHead( request, fd );
    }
    else if ( command == "PUT" )
    {
        httpPut( request, header, fd );
    }
    else if ( command == "DELETE" )
    {
        httpDelete( request, fd );
    }
    else
    {
        cout << "Unknown COMMAND: " << command << endl;
        close( fd );
    }

    // }

    close( fd );


}

int main( int argc, char *argv[] )
{

    int	listenfd, connfd;
    pthread_t tid;
    int     clilen;
    struct 	sockaddr_in cliaddr, servaddr;

    if ( argc != 3 )
    {
        printf( "Usage: httpServer <address> <port> \n" );
        return -1;
    }

    listenfd = socket( AF_INET, SOCK_STREAM, 0 );

    if ( listenfd == -1 )
    {
        fprintf( stderr,
                 "Error unable to create socket, errno = %d (%s) \n",
                 errno,
                 strerror( errno ) );
        return -1;
    }

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
        clilen = sizeof( cliaddr );
        connfd = accept( listenfd,
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

