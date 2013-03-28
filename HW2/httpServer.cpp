#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <iostream>

#include "TcpListeningSocket.h"

#define MAXLINE	32768

using namespace std;

const int backlog = 4;

float version = 1.0f;

void *httpHandler( void *socketConn );

int main( int argc, char *argv[] )
{
    if ( argc != 3 )
    {
        printf( "Usage: httpServer <http> <port> \n" );
        return -1;
    }

    version = strtof( argv[1], NULL );

    TcpListeningSocket sock( backlog, "127.0.0.1", atoi( argv[2] ) );

    cout << "Waiting for connections... " << endl;

    while ( 1 )
    {
        try
        {
            SocketConnection sc = sock.acceptConnection();
            pthread_t tid;
            pthread_create( &tid,
                            NULL,
                            &httpHandler,
                            ( void * )*sc );

        }
        catch ( exception &e )
        {
            cout << e.what() << endl;
        }
    }
}


void *httpHandler( void *socketConn )
{
    SocketConnection *sc = ( SocketConnection * )socketConn;
    int fd = sc->getDescriptor();
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

    // HttpRequest::Handle( version, fd, request );

    /*      if ( version == 1.0f )
          {
              close( fd );
              FD_CLR( fd, &master );
              cout << "Done with connection request: " << fd << endl;
          }
    */
}