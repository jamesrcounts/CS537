#ifndef RDT_H
#define RDT_H
#include "packet.h"
#include "udt.h"

int rdt_send( int socket_descriptor,
              char *buffer,
              int buffer_length,
              int flags,
              struct sockaddr *destination_address,
              int address_length )
{
    uint32_t ackno = 0;
    uint32_t seqno = 0;
    size_t sentbytes = 0;
    size_t pos = 0;
    packet_t datapacket;
    bool ack_ok = true;
    char ack_buffer[12];
    struct sockaddr_in from_address;
    size_t from_address_length = sizeof( from_address );

    while ( 0 < buffer_length )
    {
        if ( ack_ok )
        {
            std::cerr << "Building packet for " << seqno << std::endl;
            int data_size = buffer_length < 499 ? buffer_length : 499;
            datapacket = make_pkt( ackno,
                                   seqno,
                                   buffer + pos,
                                   data_size );
            pos += data_size;
            buffer_length -= data_size;
            seqno = ( seqno + 1 ) % 2;
            bzero( ack_buffer, 12 );
        }

        sentbytes += udt_send( socket_descriptor,
                               datapacket,
                               flags,
                               destination_address,
                               address_length );
        std::cerr << "Sent packet, waiting for ack..." << std::endl;
        ssize_t ack_size = udt_recv( socket_descriptor,
                                     ack_buffer,
                                     12,
                                     0,
                                     ( sockaddr * )&from_address,
                                     ( socklen_t * )&from_address_length );
        std::cerr << "Read " << ack_size << "bytes." << std::endl;

        packet_t ack;
        bzero( &ack, sizeof( ack ) );
        ssize_t ackpktsize = make_pkt( ack, ack_buffer, ack_size );
        ack_ok = ( -1 != ackpktsize && ack.ackno == seqno );
        std::cout << "got the ack i wanted." << std::endl;
    }

    return sentbytes;
}


#endif