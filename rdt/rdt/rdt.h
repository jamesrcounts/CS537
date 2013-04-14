#ifndef RDT_H
#define RDT_H 

#include <stdint.h>

packet_t rdt_loadpacket(const char *buffer, int buffer_length){
    packet_t p;
    bzero( ( void * )&p, sizeof( p ) );
    build_packet(&p,10,101, buffer, buffer_length );
    return p;	
}

int rdt_unloadpacket(const packet_t &p, char * buffer,int  buffer_length)
{
	int numbytes = 0;

	uint16_t checksum = packet_checksum(p);
	if(checksum != 0){
		return -1;
	} 

    // do checksum if ok we eventually return seq, if not ok, return -1 right now
    // possibly set errno

    // checksum now known to be ok

    // packet_t rp;
    // fill_packet( recv_buffer, &rp );
    // memcpy( buffer, rp.data, buffer_length );
    // return numbytes - 12;

    return numbytes;
}


int rdt_send( int socket_descriptor,
              char *buffer,
              int buffer_length,
              int flags,
              struct sockaddr *destination_address,
              int address_length )
{
	packet_t p = rdt_loadpacket(buffer, buffer_length);
    return sendto( socket_descriptor,
                           (void *)&p,
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
              int *address_length  )
{
    char recv_buffer[512];
    bzero( recv_buffer, 512 );
    int numbytes = recvfrom( socket_descriptor,
                             recv_buffer,
                             512,
                             flags,
                             from_address,
                             ( socklen_t * )address_length );

    // do checksum if ok we eventually return seq, if not ok, return -1 right now
    // possibly set errno

    // checksum now known to be ok

    packet_t rp;
    fill_packet( recv_buffer, &rp );

    memcpy( buffer, rp.data, ntohs( rp.len ) - 12 );
    return numbytes - 12;
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