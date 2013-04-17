#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
#include <sstream>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include "rdt.h"
#define MYPORT "4950"   // the port users will be connecting to

#define MAXBUFLEN 2048

int sockfd;
void my_handler( int s )
{
    printf( "Caught signal %d\n", s );
    rdt_close( sockfd );
    exit( 1 );
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr( struct sockaddr *sa )
{
    if ( sa->sa_family == AF_INET )
    {
        return &( ( ( struct sockaddr_in * )sa )->sin_addr );
    }

    return &( ( ( struct sockaddr_in6 * )sa )->sin6_addr );
}

std::string Dump( const packet_t &p )
{
    std::stringstream out;
    out << "cksum: " << p.cksum << std::endl <<
        "len: " << ntohs( p.len ) << std::endl <<
        "ackno: " << ntohl( p.ackno ) << std::endl <<
        "seqno: " << ntohl( p.seqno )  << std::endl ;

    char buffer[512];
    bzero( buffer, 512 );
    memcpy( buffer, p.data, ntohs( p.len ) - 12 );
    out << "data: " << buffer << std::endl;
    return out.str();
}

int main( void )
{
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_handler;
    sigemptyset( &sigIntHandler.sa_mask );
    sigIntHandler.sa_flags = 0;
    sigaction( SIGINT, &sigIntHandler, NULL );

    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char buf[MAXBUFLEN];
    bzero( buf, MAXBUFLEN );
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];

    memset( &hints, 0, sizeof hints );
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ( ( rv = getaddrinfo( NULL, MYPORT, &hints, &servinfo ) ) != 0 )
    {
        fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( rv ) );
        return 1;
    }

    // loop through all the results and bind to the first we can
    for ( p = servinfo; p != NULL; p = p->ai_next )
    {
        if ( ( sockfd = rdt_socket( p->ai_family, p->ai_socktype,
                                    p->ai_protocol ) ) == -1 )
        {
            perror( "listener: socket" );
            continue;
        }

        if ( rdt_bind( sockfd, p->ai_addr, p->ai_addrlen ) == -1 )
        {
            close( sockfd );
            perror( "listener: bind" );
            continue;
        }

        break;
    }

    if ( p == NULL )
    {
        fprintf( stderr, "listener: failed to bind socket\n" );
        return 2;
    }

    freeaddrinfo( servinfo );

    while ( 1 )
    {
        printf( "listener: waiting to recvfrom...\n" );
        addr_len = sizeof their_addr;
        numbytes = rdt_recv( sockfd,
                             buf,
                             MAXBUFLEN,
                             0,
                             ( struct sockaddr * )&their_addr,
                             ( int * )&addr_len );

        if ( numbytes == -1 )
        {
            perror( "recvfrom" );
            exit( 1 );
        }

        printf( "listener: got packet from %s\n",
                inet_ntop( their_addr.ss_family,
                           get_in_addr( ( struct sockaddr * )&their_addr ),
                           s, sizeof s ) );
        printf( "listener: packet is %d bytes long\n", numbytes );


        printf( "listener: packet contains \n%s\n", buf );

    }

    close( sockfd );

    return 0;
}