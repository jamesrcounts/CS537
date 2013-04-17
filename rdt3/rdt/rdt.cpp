#include "rdt.h"

#define PACKET_SIZE 512

#include <poll.h>

uint16_t checksum( const void *data,  size_t count )
{
    char *addr = ( char * )data;
    register uint32_t sum = 0;

    while ( count > 1 )
    {
        sum = sum + *( ( uint16_t * ) addr );
        addr += 2;
        count = count - 2;
    }

    if ( count > 0 )
    {
        sum = sum + *( ( unsigned char * ) addr );
    }

    while ( sum >> 16 )
    {
        sum = ( sum & 0xFFFF ) + ( sum >> 16 );
    }

    return( ~sum );
}

void make_pkt( packet_t &p,
               const uint32_t ackno,
               const uint32_t seqno,
               const char *data,
               const size_t data_size )
{
    static size_t header_size = 12;
    size_t packet_size = header_size + data_size;

    bzero( &p, sizeof( p ) );
    memcpy( &p.data, data, data_size );
    p.seqno = htonl( seqno );
    p.ackno = htonl( ackno );
    p.len = htons( packet_size );
    p.cksum = checksum( &p, packet_size );
}

ssize_t make_pkt( packet_t &p, const char *buffer, size_t buffer_size )
{
    uint16_t sum = checksum( buffer, buffer_size );

    if ( buffer_size < 12 ||  sum != 0 )
    {
        printf( "Unacceptable packet size: %d, checksum %d\n", buffer_size, sum );
        return -1;
    }

    uint16_t len;
    memcpy( &len, buffer + 2, 2 );
    len = ntohs( len );

    bzero( &p, sizeof( p ) );
    memcpy( &p, buffer, len );

    return len;
}



bool seeded = false;

int udt_recv( int socket_descriptor,
              char *buffer,
              int buffer_length,
              int flags,
              struct sockaddr *from_address,
              int *address_length )
{


    int numbytes = recvfrom( socket_descriptor,
                             buffer,
                             buffer_length ,
                             flags,
                             from_address,
                             ( socklen_t * )address_length );

    if ( !seeded )
    {
        srand( time( NULL ) );
    }

    int chaos = rand() % 100 + 1;

    if ( 80 <= chaos )
    {
        int x = buffer_length % chaos;
        buffer[x] = rand() % 256;
    }

    return numbytes;
}



ssize_t udt_send( int socket_descriptor,
                  const packet_t &p,
                  int flags,
                  struct sockaddr *destination_address,
                  int address_length )
{
    packet_t u;
    memcpy( &u, &p, sizeof( u ) );

    if ( !seeded )
    {
        srand( time( NULL ) );
    }

    int chaos = rand() % 100 + 1;

    if ( 80 <= chaos )
    {
        int x = sizeof( u ) % chaos;
        char *pp = ( char * )&u;
        pp[x] = rand() % 256;
    }


    return sendto( socket_descriptor, &u, ntohs( u.len ), flags,
                   destination_address, address_length );
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

int rdt_recv( int socket_descriptor,
              char *buffer,
              int buffer_length,
              int flags,
              struct sockaddr *from_address,
              int *address_length )
{
    uint32_t seqno = 0;
    int totbytes = 0;
    struct pollfd pdfs[1];
    size_t pos = 0;

    while ( 0 < buffer_length )
    {
        char recv_buffer[PACKET_SIZE];
        bzero( recv_buffer, PACKET_SIZE );
        int numbytes = udt_recv( socket_descriptor,
                                 recv_buffer,
                                 PACKET_SIZE,
                                 flags,
                                 from_address,
                                 address_length );
        totbytes += numbytes;

        if ( numbytes == -1 )
        {
            printf( "%s\n", "no data" );
            continue;
        }

        packet_t recv;

        if ( -1 == make_pkt( recv, recv_buffer, numbytes ) )
        {
            printf( "%s\n", "corrupt packet" );
            continue;
        }

        uint32_t rseqno = ntohl( recv.seqno );

        if ( rseqno != seqno )
        {
            printf( "%s\n", "wrong seqno" );
            continue;
        }

        seqno = ( seqno + 1 ) % 2;

        packet_t ack;
        bzero( &ack, sizeof( ack ) );
        ack.ackno = recv.seqno;
        ack.len = htons( 12 );
        ack.cksum = checksum( &ack, 12 );

        udt_send( socket_descriptor,
                  ack,
                  0,
                  from_address,
                  *address_length );

        uint16_t data_len = ntohs( recv.len ) - 12;
        size_t data_size = buffer_length <= data_len ?
                           buffer_length  :
                           data_len;

        memcpy( buffer + pos, recv.data, data_size );
        buffer_length -= data_size;
        pos += data_size;

        pdfs[0].fd = socket_descriptor;
        pdfs[0].events = POLLIN;

        if ( poll( pdfs, 1, 1000 ) < 1 )
        {
            break;
        }

        if ( !( pdfs[0].revents & POLLIN ) )
        {
            break;
        }
    }

    return totbytes;
}

int rdt_send( int socket_descriptor,
              char *buffer,
              int buffer_length,
              int flags,
              struct sockaddr *destination_address,
              int address_length )
{
    uint32_t ackno = 0;
    uint32_t seqno = 0;
    struct pollfd pdfs[1];
    size_t pos = 0;
    ssize_t numbytes = 0;

    while ( 0 < buffer_length )
    {
        size_t data_size = 499 < buffer_length ? 499 : buffer_length;

        packet_t p;
        make_pkt( p, ackno, seqno, buffer + pos, data_size );
        pos += data_size;
        buffer_length -= data_size;

        for ( int retry = 0; retry < 5; ++retry )
        {
            numbytes += udt_send( socket_descriptor,
                                  p,
                                  flags,
                                  destination_address,
                                  address_length );
            pdfs[0].fd = socket_descriptor;
            pdfs[0].events = POLLIN;

            if ( poll( pdfs, 1, 1000 ) < 1 )
            {
                printf( "%s\n", "no activity" );

                if ( retry == 4 )
                {
                    return -1;
                }

                continue;
            }

            if ( !( pdfs[0].revents & POLLIN ) )
            {
                printf( "%s\n", "no incoming activity" );

                if ( retry == 4 )
                {
                    return -1;
                }

                continue;
            }

            char ackbuf[12];
            bzero( ackbuf, 12 );
            ssize_t ackbytes = udt_recv( socket_descriptor,
                                         ackbuf,
                                         12,
                                         0,
                                         NULL,
                                         NULL );

            // uint16_t sum = checksum( ackbuf, ackbytes );

            packet_t ack;

            if ( -1 == make_pkt( ack, ackbuf, ackbytes ) )
            {
                printf( "%s\n", "corrupt packet while waiting for ack" );

                if ( retry == 4 )
                {
                    return -1;
                }

                continue;
            }

            // make_pkt( ack, ackbuf, 12 );

            if ( ack.ackno  == p.seqno )
            {

                seqno = ( seqno + 1 ) % 2;
                break;
            }

            printf( "%s\n", "wrong ackno" );

            if ( retry == 4 )
            {
                return -1;
            }
        }
    }

    return numbytes;
}

int rdt_socket( int address_family,
                int type,
                int protocol )
{
    return socket( address_family, type, protocol );
}



