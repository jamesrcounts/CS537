#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include "rdt.h"

#define SERVERPORT "4950"

using namespace std;

int main( int argc, char *argv[] )
{

    if ( argc != 3 )
    {
        fprintf( stderr, "usage: talker hostname message\n" );
        exit( 1 );
    }

    struct addrinfo hints, *servinfo, *p;

    memset( &hints, 0, sizeof hints );

    hints.ai_family = AF_UNSPEC;

    hints.ai_socktype = SOCK_DGRAM;

    int rv;

    if ( ( rv = getaddrinfo( argv[1], SERVERPORT, &hints, &servinfo ) ) != 0 )
    {
        fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( rv ) );
        return 1;
    }

    // loop through all the results and make a socket
    int sockfd;

    for ( p = servinfo; p != NULL; p = p->ai_next )
    {
        if ( ( sockfd = rdt_socket( p->ai_family, p->ai_socktype,
                                    p->ai_protocol ) ) == -1 )
        {
            perror( "talker: socket" );
            continue;
        }

        break;
    }

    if ( p == NULL )
    {
        fprintf( stderr, "talker: failed to bind socket\n" );
        return 2;
    }


    int numbytes;

    string path( argv[2] );
    ifstream file( path.c_str(), ios::in | ios::ate );

    if ( file.is_open() )
    {
        int size( file.tellg() );
        file.seekg( 0, ios::beg );
        char block[size + 1];
        bzero( block, size + 1 );
        file.read( block, size );
        file.close();

        if ( ( numbytes = rdt_send( sockfd, block, size, 0,
                                    p->ai_addr, p->ai_addrlen ) ) == -1 )
        {
            perror( "talker: sendto" );
            exit( 1 );
        }
    }
    else
    {
        perror( "talker: readfile" );
        exit( 1 );
    }



    freeaddrinfo( servinfo );

    printf( "talker: sent %d bytes to %s\n", numbytes, argv[1] );
    rdt_close( sockfd );

    return 0;
}