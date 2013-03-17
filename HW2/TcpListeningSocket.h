#ifndef TCPLISTENINGSOCKET_H
#define TCPLISTENINGSOCKET_H

#include "TcpSocket.h"
#include "SocketConnection.h"

class TcpListeningSocket
{
private:
    TcpSocket tcp_skt;
public:
    TcpListeningSocket( int backlog = 4,
                        string address_spec = "",
                        uint16_t port = 1025 )
        : tcp_skt( address_spec, port )
    {
        tcp_skt.bindToAddress();
        tcp_skt.makePassive( backlog );
    }

    SocketConnection &acceptConnection()
    {
        struct sockaddr_in client_address;
        uint address_len = sizeof( client_address );
        int connfd = accept( tcp_skt.getDescriptor(),
                             ( struct sockaddr * )&client_address,
                             &address_len );

        if ( connfd == -1 )
        {
            int error = errno;
            throw InternetSocketException( error );
        }

        SocketConnection *t = new SocketConnection( connfd,
                client_address );
        return *t;
    }
};

#endif