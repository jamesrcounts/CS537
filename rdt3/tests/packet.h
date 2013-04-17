#ifndef PACKET_H
#define PACKET_H

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

struct packet
{
    uint16_t cksum; /* Ack and Data */
    uint16_t len;   /* Ack and Data */
    uint32_t ackno; /* Ack and Data */
    uint32_t seqno; /* Data only */
    char data[500]; /* Data only; Not always 500 unsigned chars, can be less */
};

typedef struct packet packet_t;

packet_t make_pkt( const uint32_t ackno,
                   const uint32_t seqno,
                   const char *data,
                   const size_t data_size )
{
    static size_t header_size = 12;
    size_t packet_size = header_size + data_size;

    packet_t p;
    bzero( &p, sizeof( p ) );

    memcpy( &p.data, data, data_size );
    p.seqno = htonl( seqno );
    p.ackno = htonl( ackno );
    p.len = htons( packet_size );
    p.cksum = checksum( &p, packet_size );

    return p;
}

ssize_t make_pkt( packet_t &p, const char *buffer, size_t buffer_size )
{
    if ( buffer_size < 12 || checksum( buffer, buffer_size ) != 0 )
    {
        return -1;
    }

    uint16_t len;
    memcpy( &len, buffer + 2, 2 );
    len = ntohs( len );

    bzero( &p, sizeof( p ) );
    memcpy( &p, buffer, len );
    return len;
}

#endif