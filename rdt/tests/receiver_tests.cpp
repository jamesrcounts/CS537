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

bool cancel_thread = false;
void *ping( void *state );
void *send_file( void *state );
void send_rdtfile( const char *filename,
                   const char *address,
                   int port );

void send_rdtmessage( const char *message,
                      size_t size,
                      const char *address,
                      int port );
int get_rdtlistener( const char *address, int port );

Context( DescribeRdtRecv )
{
    Spec( ItCanReadRdtPacketsOffTheWire )
    {
        cancel_thread = false;
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

        int numbytes = rdt_recv( listenfd,
                                 buffer,
                                 512,
                                 0,
                                 ( struct sockaddr * )&cliaddr,
                                 &clilen );
        AssertThat( numbytes, !Equals( -1 ) );
        rdt_close( listenfd );
        Approvals::Verify( string( buffer ) );
        cancel_thread = true;
        pthread_join( tid, NULL );
    }

    Spec( ItCanReadRdtStreamsOffTheWire )
    {
        cancel_thread = false;
        pthread_t tid;
        pthread_create( &tid,
                        NULL,
                        &send_file,
                        ( void * )NULL );
        int listenfd = get_rdtlistener( "127.0.0.1", 3491 );
        char buffer[512];
        bzero( buffer, 512 );
        struct sockaddr_in cliaddr;
        int clilen = sizeof( cliaddr );
        int numbytes = rdt_recv( listenfd,
                                 buffer,
                                 512,
                                 0,
                                 ( struct sockaddr * )&cliaddr,
                                 &clilen );
        AssertThat( numbytes, !Equals( -1 ) );
        rdt_close( listenfd );
        cancel_thread = true;
        Approvals::Verify( string( buffer ) );
        pthread_join( tid, NULL );
    }

    Spec( ItChecksTheCheckSum )
    {
        packet_t p = rdt_loadpacket( "Hello Michael", 14 );
        p.cksum = 103;
        char buffer[14];
        int buffer_length = 14;
        int numbytes = rdt_unloadpacket( ( char * )&p,
                                         htons( p.len ),
                                         buffer,
                                         buffer_length );
        Assert::That( numbytes, Equals( -1 ) );
    }

    Spec( AValidPacketCanBeUnloaded )
    {
        packet_t p = rdt_loadpacket( "Hello Jim", 10 );
        int buffer_length = 10;
        char buffer[buffer_length];
        rdt_unloadpacket( ( char * )&p, htons( p.len ), buffer, buffer_length );
        Approvals::Verify( string( buffer ) );
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

        if ( cancel_thread )
        {
            break;
        }

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

    if ( ( sockfd = rdt_socket( AF_INET, SOCK_DGRAM, 0 ) ) == -1 )
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

    rdt_close( sockfd );

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

void *send_file( void *state )
{
    for ( int i = 0; i < 10; ++i )
    {
        send_rdtfile( "Makefile",
                      "127.0.0.1",
                      3491 );

        if ( cancel_thread )
        {
            break;
        }

        sleep( i );
    }

    return NULL;
}

void send_rdtfile( const char *filename,
                   const char *address,
                   int port )
{
    char buf[512];
    bzero( buf, 512 );
    ssize_t readlink_ok = readlink( "/proc/self/exe", buf, 511 );

    string dir( "." );

    if ( readlink_ok != -1 )
    {
        dir = string( buf );
    }

    unsigned slash = dir.find_last_of( "/" );

    if ( slash != std::string::npos )
    {
        dir = dir.substr( 0, slash );
    }

    string path = dir + "/pub/" + filename;

    ifstream file( path.c_str(), ios::in | ios::binary | ios::ate );

    if ( file.is_open() )
    {
        int size( file.tellg() );
        file.seekg( 0, ios::beg );
        char block[size];
        bzero( block, size );
        file.read( block, size );
        file.close();
        send_rdtmessage( block, size, address, port );
    }
    else
    {
        send_rdtmessage( path.c_str(), path.size(), address, port );
    }
}
