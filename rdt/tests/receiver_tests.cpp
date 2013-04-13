#include <errno.h>
#include <igloo/igloo.h>
#include <approvals/Approvals.h>
#include <stdint.h>
#include <iostream>
#include <sstream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

using namespace std;

using namespace igloo;

struct packet
{
    uint16_t cksum; /* Ack and Data */
    uint16_t len;   /* Ack and Data */
    uint32_t ackno; /* Ack and Data */
    uint32_t seqno; /* Data only */
    char data[500]; /* Data only; Not always 500 bytes, can be less */
};
typedef struct packet packet_t;

ostream &operator << ( ostream &out, packet_t &a )
{
    out << "cksum: " << a.cksum << endl;
    out << "len: " << a.len << endl;
    out << "ackno: " << a.ackno << endl;
    out << "seqno: " << a.seqno << endl;
    out << "data: " << a.data << endl;
    return out;
}

uint16_t packet_checksum( const packet_t &p )
{
    uint32_t count = sizeof( p );
    unsigned char *packet_data = new unsigned char[count];
    bzero( packet_data, count );
    memcpy( packet_data, ( void * )&p, count );

    register uint32_t sum = 0;

    // Main summing loop
    while ( count > 1 )
    {
        sum += ( uint32_t ) * packet_data;
        packet_data++;
        count = count - 2;
    }

    // Add left-over byte, if any
    if ( count > 0 )
    {
        sum = sum + *( ( uint32_t * ) packet_data );
    }

    // Fold 32-bit sum to 16 bits
    while ( sum >> 16 )
    {
        sum = ( sum & 0xFFFF ) + ( sum >> 16 );
    }

    return( ~sum );
}

void build_packet( const char *data, uint32_t ack, uint32_t seq, packet_t *p )
{
    size_t size = strlen( data );
    p->len = htons( size + 12 );
    p->ackno = htonl( ack );
    p->seqno = htonl( seq );
    memcpy( p->data, data, size );

    p->cksum = packet_checksum( *p );
}

void fill_packet( char *data, packet_t *p )
{
    uint16_t ck;
    memcpy( ( void * )&ck, data, 2 );
    p->cksum = ck;

    uint16_t l;
    memcpy( ( void * )&l, data + 2, 2 );
    p->len = l;

    uint32_t ack;
    memcpy( ( void * )&ack, data + 4, 4 );
    p->ackno = ack;

    uint32_t seq;
    memcpy( ( void * )&seq, data + 8, 4 );
    p->seqno = seq;

    uint16_t size = ntohs( l ) - 12;
    memcpy( p->data, data + 12, size );
}

int rdt_recv( int socket_descriptor /* udp socket */,
              char *buffer /* payload goes here */,
              int buffer_length /* max size buffer can take */,
              int flags /* pass to recvfrom */,
              struct sockaddr *from_address /* get from udp */,
              int *address_length /* get from udp */ )
{
    char recv_buffer[512];
    bzero( recv_buffer, 512 );
    int numbytes = recvfrom( socket_descriptor,
                             recv_buffer,
                             512,
                             0,
                             from_address,
                             ( socklen_t * )address_length );

    if ( numbytes == -1 )
    {
        perror( "recvfrom" );
        throw 1;
    }

    packet_t rp;
    fill_packet( recv_buffer, &rp );

    memcpy( buffer, rp.data, ntohs( rp.len ) - 12 );

    // do checksum if ok we eventually return seq, if not ok, return -1 right now
    // possibly set errno

    // checksum now known to be ok
    // rdt packet <- fillpacket
    // now read our rdt packet, into "thier buffer"
    // thierbuffer <- packet.payload

    return numbytes - 12;
}

void *ping( void *state )
{
    for ( int i = 0; i < 10; ++i )
    {
        int sockfd;
        int numbytes;

        struct  sockaddr_in servaddr;
        bzero( &servaddr, sizeof( servaddr ) );
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr =  inet_addr( "127.0.0.1" );
        servaddr.sin_port =                    htons( 3490 ) ;

        if ( ( sockfd = socket( AF_INET, SOCK_DGRAM, 0 ) ) == -1 )
        {
            perror( "talker: socket" );
            exit( 1 );
        }

        packet_t p;
        bzero( ( void * )&p, sizeof( p ) );
        build_packet( "Hello Michael", 10, 101, &p );

        uint32_t count = sizeof( p );
        char *packet_data = new char[count];
        bzero( packet_data, count );
        memcpy( packet_data, ( void * )&p, count );

        numbytes = sendto( sockfd,
                           packet_data,
                           ntohs( p.len ),
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
        sleep( 1 );
    }

    return NULL;
}

int rdt_socket( int address_family,
                int type,
                int protocol )
{
    return socket( address_family, type, protocol );
}

int rdt_bind( int socket_descriptor,
              const struct sockaddr *local_address,
              socklen_t address_length )
{
    return bind( socket_descriptor, local_address, address_length );
}

int rdt_close( int socket_descriptor )
{
    return close( socket_descriptor );
}

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
        int listenfd = rdt_socket( AF_INET, SOCK_DGRAM, 0 );

        if ( listenfd == -1 )
        {
            fprintf( stderr,
                     "Error unable to create socket, errno = %d (%s) \n",
                     errno,
                     strerror( errno ) );
            throw 1;
        }

        struct  sockaddr_in servaddr;

        bzero( &servaddr, sizeof( servaddr ) );

        servaddr.sin_family        = AF_INET;

        servaddr.sin_addr.s_addr   = inet_addr( "127.0.0.1" );

        servaddr.sin_port          = htons( 3490 );

        int bind_result = rdt_bind( listenfd,
                                    ( struct sockaddr * )&servaddr,
                                    sizeof( servaddr ) );

        if ( bind_result == -1 )
        {
            fprintf( stderr,
                     "Error binding to socket, errno = %d (%s) \n",
                     errno,
                     strerror( errno ) );
            throw 1;

        }

        char buffer[512];
        bzero( buffer, 512 );
        struct  sockaddr_in cliaddr;
        int clilen = sizeof( cliaddr );

        printf( "listener: waiting to recvfrom...\n" );
        int num_bytes = rdt_recv( listenfd,
                                  buffer,
                                  512,
                                  0,
                                  ( struct sockaddr * )&cliaddr,
                                  &clilen );

        cout << "Recieved: " << num_bytes << " bytes" << endl;

        rdt_close( listenfd );
        Approvals::Verify( string( buffer ) );
    }
};


Context( DescribeAPacket )
{
    Spec( ItHasTheFollowingFields )
    {
        packet_t p;
        bzero( ( void * ) &p, sizeof( p ) );

        p.cksum = 2;
        p.len = 6;
        p.ackno = 4092;
        p.seqno = 8903;

        char data[6] = "Hello";
        memcpy( p.data, data, 6 );

        stringstream s;
        s << p;

        Approvals::Verify( s.str() );
    };

    Spec( APacketCanBeBuiltByBuildPacket )
    {
        packet_t p;
        bzero( ( void * ) &p, sizeof( p ) );
        build_packet( "Hello World!", 10, 100, &p );

        stringstream s;
        s << p;

        Approvals::Verify( s.str() );
    }

    Spec( APacketCanBeParsedByParsePacket )
    {
        packet_t p;
        bzero( ( void * )&p, sizeof( p ) );
        build_packet( "Hello Michael", 10, 101, &p );

        uint32_t count = sizeof( p );
        char *packet_data = new char[count];
        bzero( packet_data, count );
        memcpy( packet_data, ( void * )&p, count );

        packet_t q;
        bzero( ( void * )&q, sizeof( q ) );
        fill_packet( packet_data, &q );
        stringstream s;
        s << q;

        Approvals::Verify( s.str() );
    }
};

Context( DescribeACheckSum )
{
    Spec( ItDescribesThePacket )
    {
        packet_t p;
        bzero( ( void * ) &p, sizeof( p ) );

        p.len = 7;
        p.ackno = 4092;
        p.seqno = 8903;

        char data[7] = "Hello!";
        memcpy( p.data, data, 7 );

        p.cksum = packet_checksum( p );

        stringstream s;
        s << p;

        Approvals::Verify( s.str() );

    };
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