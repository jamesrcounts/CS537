#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <arpa/inet.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdint.h>
#include "rdt.h"
#include "udt.h"

using namespace std;
namespace Data
{
std::string Dump( const struct sockaddr_in &a )
{

    std::stringstream out;
    out << "Family: " << a.sin_family << std::endl <<
        "Address: " << inet_ntoa( a.sin_addr ) << std::endl <<
        "Port: " << ntohs( a.sin_port ) << std::endl;
    return out.str();
}
std::string Dump( const struct sockaddr_in *a )
{
    return Dump( *a );
}

std::string Dump( const struct sockaddr *a )
{
    return Dump( ( sockaddr_in * )  a );
}

std::string Dump( const packet_t &p )
{
    std::stringstream out;
    out << "cksum: " << p.cksum << std::endl <<
        "len: " << ntohs( p.len ) << std::endl <<
        "ackno: " << ntohl( p.ackno ) << std::endl <<
        "seqno: " << ntohl( p.seqno )  << std::endl <<
        "data: " << p.data << std::endl;
    return out.str();
}

std::string Dump( const std::vector<packet_t>& vp )
{
    std::stringstream out;

    for ( std::vector<packet_t>::const_iterator pit = vp.begin();
            pit != vp.end();
            ++pit )
    {
        out << Dump( *pit ) << std::endl;
    }

    return out.str();
}
}

#include <igloo/igloo.h>
#include <approvals/Approvals.h>
using namespace igloo;

struct sockaddr_in get_address( const char *addr, const int port )
{
    struct  sockaddr_in local_address;
    socklen_t address_length = sizeof( local_address );
    bzero( &local_address, address_length );
    local_address.sin_family        = AF_INET;
    local_address.sin_addr.s_addr   = inet_addr( addr );
    local_address.sin_port          = htons( port );
    return local_address;
}

int get_listener( const char *addr, const int port )
{
    int socket_descriptor = socket( AF_INET, SOCK_DGRAM, 0 );

    struct  sockaddr_in local_address = get_address( addr, port );
    socklen_t address_length = sizeof( local_address );

    bind( socket_descriptor,
          ( sockaddr * ) &local_address,
          address_length );

    return socket_descriptor;
}

int get_packetdata( const int socket_descriptor,
                    char *buffer,
                    const size_t buffer_size,
                    sockaddr *from_address,
                    socklen_t *address_length )
{
    bzero( buffer, buffer_size );

    int recv_ok = recvfrom( socket_descriptor,
                            buffer,
                            buffer_size,
                            0,
                            from_address,
                            address_length );
    return recv_ok;
}

bool stop_sending = false;

void *send_test_data( void *state )
{
    char *data = ( char * )state;
    size_t buffer_length = strlen( data );
    int socket_descriptor = socket( AF_INET, SOCK_DGRAM, 0 );
    struct sockaddr_in destination_address = get_address( "127.0.0.1", 3491 );
    socklen_t address_length = sizeof( destination_address );

    for ( int i = 0; i < 10; ++i )
    {
        if ( stop_sending )
        {
            break;
        }

        rdt_send( socket_descriptor,
                  data,
                  buffer_length,
                  0,
                  ( sockaddr * )&destination_address,
                  address_length );
    }

    close( socket_descriptor );
    return NULL;
}

void *send_big_data( void *state )
{
    char data[768];
    bzero( data, 768 );
    memset( data, '-', 767 );
    memcpy( data, "Hello Michael\n", 14 );
    memcpy( data + 512, "And Jim\n", 8 );
    size_t buffer_length = 768;
    int socket_descriptor = socket( AF_INET, SOCK_DGRAM, 0 );
    struct sockaddr_in destination_address = get_address( "127.0.0.1", 3492 );
    socklen_t address_length = sizeof( destination_address );
    bind( socket_descriptor,
          ( sockaddr * ) &destination_address,
          address_length );


    for ( int i = 0; i < 10; ++i )
    {
        sleep( i );

        if ( stop_sending )
        {
            break;
        }

        rdt_send( socket_descriptor,
                  data,
                  buffer_length,
                  0,
                  ( sockaddr * )&destination_address,
                  address_length );
    }

    close( socket_descriptor );
    return NULL;
}

void *send_test_packet( void *state )
{
    packet_t *p = ( packet_t * )state;
    int socket_descriptor = socket( AF_INET, SOCK_DGRAM, 0 );
    struct sockaddr_in destination_address = get_address( "127.0.0.1", 3490 );
    socklen_t address_length = sizeof( destination_address );

    for ( int i = 0; i < 10; ++i )
    {
        if ( stop_sending )
        {
            break;
            return NULL;
        }

        udt_send( socket_descriptor,
                  *p,
                  0,
                  ( sockaddr * )&destination_address,
                  address_length );
    }

    close( socket_descriptor );
    return NULL;
}

Context( DescribeAPacket )
{
    Spec( APacketConsistsOfDataAndHeaders )
    {
        size_t data_size = 6;
        char data[data_size];
        bzero( data, data_size );
        memcpy( data, "Hello", data_size - 1 );
        uint32_t seqno = 1;
        uint32_t ackno = 1;
        packet_t p = make_pkt( ackno, seqno, data, data_size );
        Assert::That( checksum( &p, ntohs( p.len ) ), Equals( 0 ) );
        Approvals::Verify( Data::Dump( p ) );
    }

    Spec( YouCanSendAPacketwithUdtSend )
    {
        packet_t p = make_pkt( 0, 0, "Hello Jim", 10 );
        stop_sending = false;

        pthread_t tid;
        pthread_create( &tid, NULL, &send_test_packet, &p );

        int socket_descriptor = get_listener( "127.0.0.1", 3490 );

        sockaddr_in from_address;
        bzero( &from_address, sizeof( from_address ) );
        socklen_t address_length = sizeof( from_address );
        char recv_buffer[512];
        int numbytes = get_packetdata(
                           socket_descriptor,
                           recv_buffer,
                           512,
                           ( sockaddr * )&from_address,
                           &address_length );

        AssertThat( numbytes, IsGreaterThan( -1 ) );
        AssertThat( checksum( recv_buffer, numbytes ), Equals( 0 ) );

        packet_t r;
        int pakbytes = make_pkt( r, recv_buffer, numbytes );

        AssertThat( pakbytes, IsGreaterThan( -1 ) );
        Approvals::Verify( Data::Dump( r ) );
        close( socket_descriptor );
        stop_sending = true;
    }

    Spec( YouCanSendDataWithRdtSend )
    {
        char data[10] =  "Hello Jim";
        stop_sending = false;

        pthread_t tid;
        pthread_create( &tid, NULL, &send_test_data, data );
        int socket_descriptor = get_listener( "127.0.0.1", 3491 );
        char recv_buffer[512];
        sockaddr_in from_address;
        bzero( &from_address, sizeof( from_address ) );
        socklen_t address_length;
        int numbytes = get_packetdata(
                           socket_descriptor,
                           recv_buffer,
                           512,
                           ( sockaddr * )&from_address,
                           &address_length );

        AssertThat( numbytes, IsGreaterThan( -1 ) );
        AssertThat( checksum( recv_buffer, numbytes ), Equals( 0 ) );
        packet_t r;
        int pakbytes = make_pkt( r, recv_buffer, numbytes );

        AssertThat( pakbytes, IsGreaterThan( -1 ) );
        Approvals::Verify( Data::Dump( r ) );
        stop_sending = true;
        close( socket_descriptor );
    }

    Spec( RdtSendProvidesAStreamAbstraction )
    {
        stop_sending = false;

        pthread_t tid;
        pthread_create( &tid, NULL, &send_big_data, NULL );
        int socket_descriptor = get_listener( "127.0.0.1", 3492 );

        std::vector<packet_t> packets;

        for ( int i = 0; i < 2; ++i )
        {
            bool recv_again = true;
            packet_t r;
            sockaddr_in from_address;
            socklen_t address_length = sizeof( from_address );

            while ( recv_again )
            {
                char recv_buffer[512];
                bzero( &from_address, sizeof( from_address ) );
                int numbytes = get_packetdata( socket_descriptor,
                                               recv_buffer,
                                               512 ,
                                               ( sockaddr * )&from_address,
                                               &address_length );

                AssertThat( numbytes, IsGreaterThan( -1 ) );
                AssertThat( checksum( recv_buffer, numbytes ), Equals( 0 ) );
                bzero( &r, sizeof( r ) );
                int pakbytes = make_pkt( r, recv_buffer, numbytes );

                // AssertThat( pakbytes, IsGreaterThan( 12 ) );
                recv_again = 12 == pakbytes;

                if ( recv_again )
                {
                    std::cerr << "Ate unwanted ack" << endl;
                }
            }

            std::cerr << "Got packet " << Data::Dump( r ) << std::endl;
            packets.push_back( r );

            packet_t ack;
            bzero( &ack, sizeof( ack ) );
            ack.ackno = r.seqno;
            ack.len = htons( 12 );
            ack.cksum = checksum( &ack, 12 );

            sockaddr_in ack_address;
            bzero( &ack_address, sizeof( ack_address ) );
            ack_address = get_address( "127.0.0.1", 3492 );
            ssize_t ack_ok = udt_send( socket_descriptor,
                                       ack,
                                       0,
                                       ( sockaddr * )&ack_address,
                                       address_length );
            std::cerr << "Ack send result ... " << ack_ok << std::endl;

            for ( int i = 0; i < 10; ++i )
            {
                std::cerr << "Sleeping ... " << i << std::endl;
                sleep( i );
            }
        }

        close( socket_descriptor );
        stop_sending = true;
        Approvals::Verify( Data::Dump( packets ) );
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
