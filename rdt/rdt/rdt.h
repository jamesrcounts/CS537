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


int rdt_send( int socket_descriptor,
              char *buffer,
              int buffer_length,
              int flags,
              struct sockaddr *destination_address,
              int address_length )
{
    packet_t p = rdt_loadpacket( buffer, buffer_length );
    return sendto( socket_descriptor,
                   ( void * )&p,
                   ntohs( p.len ),
                   flags,
                   destination_address,
                   ( socklen_t )address_length );
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