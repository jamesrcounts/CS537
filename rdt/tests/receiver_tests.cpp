#include <errno.h>
#include <igloo/igloo.h>
#include <approvals/Approvals.h>
#include <stdint.h>
#include <iostream>
#include <sstream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <packet.h>
#include <rdt.h>

using namespace std;
using namespace igloo;

void *ping( void *state );
void send_rdtmessage( const char *message,
                      size_t size,
                      const char *address,
                      int port );
int get_rdtlistener( const char *address, int port );

Context( DescribeRdtRecv )
{
    Spec( ItCanReadRdtPacketsOffTheWire )
    {
        pthread_t tid;
        pthread_create( &tid,
                        NULL,
                        &ping,
                        ( void * )NULL );

        sleep( 1 );

        int listenfd = get_rdtlistener( "127.0.0.1", 3490 );

        char buffer[512];
        bzero( buffer, 512 );
        struct  sockaddr_in cliaddr;
        int clilen = sizeof( cliaddr );

        rdt_recv( listenfd,
                  buffer,
                  512,
                  0,
                  ( struct sockaddr * )&cliaddr,
                  &clilen );

        rdt_close( listenfd );
        Approvals::Verify( string( buffer ) );
    }

    Spec( ItChecksTheCheckSum )
    {
        packet_t p = rdt_loadpacket( "Hello Michael", 14 );
        p.cksum = 103;
        char buffer[14];
        int buffer_length = 14;
        int numbytes = rdt_unloadpacket( p, buffer, buffer_length );
        Assert::That( numbytes, Equals( -1 ) );
    }
};

int main( int argc, char const *argv[] )
{
    DefaultTestResultsOutput output;
    TestRunner runner( output );

    IglooMetadataListener listener;
    runner.AddListener( &listener );
    MinimalProgressTestListener mpl;
    runner.AddListener( &mpl );

    return runner.Run();
}

void *ping( void *state )
{
    for ( int i = 0; i < 10; ++i )
    {
        send_rdtmessage( "Hello Michael", 14, "127.0.0.1", 3490 );
        sleep( 1 );
    }

    return NULL;
}

void send_rdtmessage( const char *message,
                      size_t size,
                      const char *address,
                      int port )
{
    int sockfd;
    struct  sockaddr_in servaddr;
    bzero( &servaddr, sizeof( servaddr ) );
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr =  inet_addr( address );
    servaddr.sin_port = htons( port ) ;

    if ( ( sockfd = socket( AF_INET, SOCK_DGRAM, 0 ) ) == -1 )
    {
        perror( "talker: socket" );
        exit( 1 );
    }

    char msg[size];
    bzero( msg, size );
    memcpy( msg, message, size );

    int numbytes = rdt_send( sockfd,
                             msg,
                             size,
                             0,
                             ( struct sockaddr * )&servaddr,
                             sizeof( servaddr ) );

    if ( numbytes == -1 )
    {
        perror( "talker: sendto" );
        exit( 1 );
    }

    printf( "talker: sent %d bytes to 127.0.0.1\n", numbytes );
    close( sockfd );

}

int get_rdtlistener( const char *address, int port )
{
    int listenfd = rdt_socket( AF_INET, SOCK_DGRAM, 0 );

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

    servaddr.sin_family        = AF_INET;

    servaddr.sin_addr.s_addr   = inet_addr( address );

    servaddr.sin_port          = htons( port );

    int bind_result = rdt_bind( listenfd,
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

    return listenfd;
}
