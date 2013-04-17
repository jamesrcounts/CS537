#ifndef UDT_H
#define UDT_H
#include "packet.h"
ssize_t udt_send( int socket_descriptor,
                  const packet_t &p,
                  int flags,
                  sockaddr *destination_address,
                  socklen_t address_length )
{
    return sendto( socket_descriptor,
                   &p,
                   ntohs( p.len ),
                   flags,
                   destination_address,
                   address_length );
}
ssize_t udt_recv( int socket_descriptor,
                  char *buffer,
                  size_t buffer_size,
                  int flags,
                  sockaddr *from_address,
                  socklen_t *address_length )
{
    std::cerr << "About to receive UDT data." << std::endl;
    ssize_t recv_ok = recvfrom( socket_descriptor,
                                buffer,
                                buffer_size,
                                0,
                                from_address,
                                address_length );
    std::cerr << "receive result: " << recv_ok << std::endl;
    return recv_ok;
}
#endif