#ifndef PACKET_H
#define PACKET_H

#include <arpa/inet.h>
#include <iostream>
#include <string.h>

struct packet
{
    uint16_t cksum; /* Ack and Data */
    uint16_t len;   /* Ack and Data */
    uint32_t ackno; /* Ack and Data */
    uint32_t seqno; /* Data only */
    char data[500]; /* Data only; Not always 500 unsigned chars, can be less */
};

typedef struct packet packet_t;

std::ostream &operator << ( std::ostream &out, packet_t &a )
{
    out << "cksum: " << a.cksum << std::endl;
    out << "len: " << ntohs( a.len ) << std::endl;
    out << "ackno: " << ntohl( a.ackno ) << std::endl;
    out << "seqno: " << ntohl( a.seqno ) << std::endl;
    out << "data: " << a.data << std::endl;
    return out;
}

uint16_t checksum( unsigned char *addr, uint32_t count )
{
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

uint16_t checksum( const packet_t &p )
{
    return checksum( ( unsigned char * )&p, ntohs( p.len ) );
}

void build_packet( packet_t *p,
                   uint32_t ack,
                   uint32_t seq,
                   const  char *data,
                   size_t size )
{
    p->len = htons( size + 12 );
    p->ackno = htonl( ack );
    p->seqno = htonl( seq );
    memcpy( p->data, data, size );

    p->cksum = checksum( *p );
}

void fill_packet( packet_t &p, const  char *data )
{
    memcpy( ( void * )&p.cksum, data, 2 );
    memcpy( ( void * )&p.len, data + 2, 2 );
    memcpy( ( void * )&p.ackno, data + 4, 4 );
    memcpy( ( void * )&p.seqno, data + 8, 4 );
    memcpy( p.data, data + 12, ntohs( p.len ) - 12 );
}
#endif