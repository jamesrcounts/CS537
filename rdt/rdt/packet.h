#ifndef PACKET_H
#define PACKET_H

#include <arpa/inet.h>
#include <iostream>
#include <string.h>

//----- Type defines ----------------------------------------------------------
typedef unsigned char      byte;    // Byte is a char
typedef unsigned short int word16;  // 16-bit word is a short int
typedef unsigned int       word32;  // 32-bit word is an int

//----- Defines ---------------------------------------------------------------
#define BUFFER_LEN        4096      // Length of buffer

//----- Prototypes ------------------------------------------------------------
word16 checksum( byte *addr, word32 count );

struct packet
{
    uint16_t cksum; /* Ack and Data */
    uint16_t len;   /* Ack and Data */
    uint32_t ackno; /* Ack and Data */
    uint32_t seqno; /* Data only */
    char data[500]; /* Data only; Not always 500 bytes, can be less */
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

uint16_t packet_checksum( const packet_t &p )
{
    return checksum( ( byte * )&p, ntohs( p.len ) );
}

void build_packet( packet_t *p,
                   uint32_t ack,
                   uint32_t seq,
                   const char *data,
                   size_t size )
{
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


//=============================================================================
//=  Compute Internet Checksum for count bytes beginning at location addr     =
//=============================================================================
word16 checksum( byte *addr, word32 count )
{
    register word32 sum = 0;

    // Main summing loop
    while ( count > 1 )
    {
        sum = sum + *( ( word16 * ) addr );
        addr += 2;
        count = count - 2;
    }

    // Add left-over byte, if any
    if ( count > 0 )
    {
        sum = sum + *( ( byte * ) addr );
    }

    // Fold 32-bit sum to 16 bits
    while ( sum >> 16 )
    {
        sum = ( sum & 0xFFFF ) + ( sum >> 16 );
    }

    return( ~sum );
}
#endif