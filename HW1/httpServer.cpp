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

float version = 1.0f;

int main( int argc, char *argv[] )
{
    if ( argc != 3 )
    {
        printf( "Usage: httpServer <http> <port> \n" );
        return -1;
    }

    version = strtof( argv[1], NULL );

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

    servaddr.sin_addr.s_addr   = inet_addr( "127.0.0.1" );

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

    fd_set master;
    FD_ZERO( &master );
    FD_SET( listenfd, &master );
    int fdmax = listenfd;

    cout << "Waiting for connections... " << endl;

    while ( 1 )
    {
        fd_set read_fds = master;

        if ( select( fdmax + 1, &read_fds, NULL, NULL, NULL ) == -1 )
        {
            fprintf( stderr,
                     "Select error, errno = %d (%s) \n",
                     errno,
                     strerror( errno ) );
            return -1;
        }

        for ( int i = 0; i <= fdmax; i++ )
        {
            if ( FD_ISSET( i, &read_fds ) )
            {
                if ( i == listenfd )
                {
                    struct  sockaddr_in cliaddr;
                    int clilen = sizeof( cliaddr );
                    int connfd = accept( listenfd,
                                         ( struct sockaddr * )&cliaddr,
                                         ( socklen_t * )&clilen );

                    if ( connfd < 0 )
                    {
                        if ( errno != EINTR )
                        {
                            fprintf( stderr,
                                     "Error connection request refused, errno = %d (%s) \n",
                                     errno,
                                     strerror( errno ) );
                        }

                        continue;
                    }

                    FD_SET( connfd, &master );

                    if ( connfd > fdmax )
                    {
                        fdmax = connfd;
                    }

                    cout << "Accepted connection request: " << connfd << endl;
                }
                else
                {

                    int fd = i;
                    size_t idx = 0;
                    size_t buffer_sz = 1024;
                    char buffer[buffer_sz];
                    bzero( buffer, buffer_sz );


                    while ( idx < ( buffer_sz - 1 ) )
                    {
                        int last_read = read( fd, &buffer[idx], 1 );

                        if ( last_read < 0 )
                        {
                            // read error, lets be done with this connection
                            close( fd );
                            FD_CLR( fd, &master );
                            cout << "Done with connection after read error: " << fd << endl;
                            break;
                        }

                        if ( last_read == 0 )
                        {
                            // disconnect by client
                            close( fd );
                            FD_CLR( fd, &master );
                            cout << "Done with connection after client disconnect: " << fd << endl;
                            break;
                        }

                        if ( last_read == 1 && buffer[idx] == '\n' )
                        {
                            break;
                        }

                        ++idx;
                    }

                    string request( buffer );

                    HttpRequest::Handle( version, fd, request );

                    /*      if ( version == 1.0f )
                          {
                              close( fd );
                              FD_CLR( fd, &master );
                              cout << "Done with connection request: " << fd << endl;
                          }
                    */


                }
            }
        }
    }
}
