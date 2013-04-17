#ifndef RDT_H
#define RDT_H
#define PACKET_SIZE 512

#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>

uint16_t checksum( const void *data,  size_t count );

struct packet
{
    uint16_t cksum; /* Ack and Data */
    uint16_t len;   /* Ack and Data */
    uint32_t ackno; /* Ack and Data */
    uint32_t seqno; /* Data only */
    char data[500]; /* Data only; Not always 500 unsigned chars, can be less */
};

typedef struct packet packet_t;

void make_pkt( packet_t &p,
               const uint32_t ackno,
               const uint32_t seqno,
               const char *data,
               const size_t data_size );

int udt_recv( int socket_descriptor,
              char *buffer,
              int buffer_length,
              int flags,
              struct sockaddr *from_address,
              int *address_length );

ssize_t udt_send( int socket_descriptor,
                  const packet_t &p,
                  int flags,
                  struct sockaddr *destination_address,
                  int address_length );

int rdt_bind( int socket_descriptor,
              const struct sockaddr *local_address,
              socklen_t address_length );

extern int rdt_close( int socket_descriptor );

int rdt_recv( int socket_descriptor,
              char *buffer,
              int buffer_length,
              int flags,
              struct sockaddr *from_address,
              int *address_length );

int rdt_send( int socket_descriptor,
              char *buffer,
              int buffer_length,
              int flags,
              struct sockaddr *destination_address,
              int address_length );

int rdt_socket( int address_family,
                int type,
                int protocol );


#endif
