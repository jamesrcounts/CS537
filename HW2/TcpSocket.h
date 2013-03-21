#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include "InternetSocket.h"
#include "SocketConnection.h"

using namespace std;

class TcpSocket : public InternetSocket
{
public:
    TcpSocket( string address_spec = "", uint16_t port = 1025 )
        : InternetSocket( address_spec, port )
    {
        create( SOCK_STREAM );
    }

    void bindToAddress()
    {
        struct sockaddr_in addr = getAddress();
        int bind_ok = bind( getDescriptor(),
                            ( struct sockaddr * )&addr,
                            sizeof( addr ) );

        if ( bind_ok == -1 )
        {
            int error = errno;
            throw InternetSocketException( error );
        }
    }

    SocketConnection &initiateConnection()
    {
        struct sockaddr_in addr = getAddress();
        int connect_ok = connect( getDescriptor(),
                                  ( struct sockaddr * )&addr,
                                  sizeof addr );

        if ( connect_ok == -1 )
        {
            int error = errno;
            throw InternetSocketException( error );
        }

        SocketConnection *t = new SocketConnection( getDescriptor(),
                addr );
        return *t;
    }

    void makePassive( int backlog )
    {
        int listen_ok = listen( getDescriptor(), backlog );

        if ( listen_ok == -1 )
        {
            int error = errno;
            throw InternetSocketException( error );
        }
    }
};

#endif