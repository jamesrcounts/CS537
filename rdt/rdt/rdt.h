#ifndef RDT_H
#define RDT_H

#include <stdint.h>

packet_t rdt_loadpacket( const  char *buffer, int buffer_length )
{
    packet_t p;
    bzero( ( void * )&p, sizeof( p ) );
    build_packet( &p, 10, 101, buffer, buffer_length );
    return p;
}

int rdt_unloadpacket( const char *udp_payload,
                      int payload_length,
                      char *buffer,
                      int  buffer_length )
{
    if ( 0 != checksum( ( unsigned char * ) udp_payload, payload_length ) )
    {
        return -1;
    }

    packet_t p;
    fill_packet( p, udp_payload );

    bzero( buffer, buffer_length );
    int l  = ntohs( p.len ) - 12;
    int numbytes = buffer_length < l ? buffer_length : l ;
    memcpy( buffer, p.data, numbytes );

    return numbytes;
}

bool wait_ack( int socket_descriptor, uint16_t ackno, uint32_t timeout )
{
    fd_set master;
    FD_ZERO( &master );
    FD_SET( socket_descriptor, &master );

    struct timeval t;
    t.tv_sec = timeout;

    if ( select( socket_descriptor + 1, &master, NULL, NULL, &t ) == -1 )
    {
        fprintf( stderr,
                 "Select error, errno = %d (%s) \n",
                 errno,
                 strerror( errno ) );
        return false;
    }

    if ( !FD_ISSET( socket_descriptor, &master ) )
    {
        return false;
    }

    struct  sockaddr_in from_address;

    int address_length = sizeof( from_address );

    char recv_buffer[12];

    bzero( recv_buffer, 12 );

    int recvbytes = rdt_recv( socket_descriptor,
                              recv_buffer,
                              12,
                              0,
                              from_address,
                              address_length );

    if ( 0 == checksum( ( unsigned char * ) recv_buffer, 12 ) )
    {
        packet_t ack;
        fill_packet( ack, recv_buffer );
        return ntohs( ack.ackno ) == ackno;
    }

    return false;
}

int rdt_send( int socket_descriptor,
              char *buffer,
              int buffer_length,
              int flags,
              struct sockaddr *destination_address,
              int address_length )
{
    int sendbytes = 0;

    while ( 0 < buffer_length )
    {
        int numbytes = 500 < buffer_length ? 500 : buffer_length;
        packet_t p = rdt_loadpacket( buffer, numbytes );
        buffer_length -= numbytes;

        sendbytes += sendto( socket_descriptor,
                             ( void * )&p,
                             ntohs( p.len ),
                             flags,
                             destination_address,
                             ( socklen_t )address_length );

        if ( !wait_ack( socket_descriptor, seqno, timeout ) )
        {
            return -1;
        }
    }

    return sendbytes;
}

int rdt_recv( int socket_descriptor,
              char *buffer,
              int buffer_length,
              int flags,
              struct sockaddr *from_address,
              int *address_length )
{
    char recv_buffer[512];
    bzero( recv_buffer, 512 );
    int numbytes = recvfrom( socket_descriptor,
                             recv_buffer,
                             512,
                             flags,
                             from_address,
                             ( socklen_t * )address_length );

    return rdt_unloadpacket( recv_buffer, numbytes, buffer, buffer_length );
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

#endif