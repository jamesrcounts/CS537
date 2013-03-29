#ifndef SOCKETCONNECTION_H
#define SOCKETCONNECTION_H

#include <arpa/inet.h>
#include <errno.h>
#include <string>
#include <strings.h>
#include <unistd.h>
#include "InternetSocket.h"

using namespace std;

class SocketConnection
{
private:
    int fd;
    struct sockaddr_in address;
public:
    SocketConnection( int connfd, struct sockaddr_in addr )
        : fd( connfd ), address( addr )
    {}

    ~SocketConnection()
    {
        close( fd );
    }

    string readMessage()
    {
        char buf[1024];
        bzero( buf, 1023 );
        ssize_t numbytes = recv( fd, buf, 1023, 0 );

        if ( numbytes == -1 )
        {
            int error = errno;
            throw InternetSocketException( error );
        }

        return string( buf );
    }

    void sendMessage( string message )
    {
        ssize_t send_ok = send( fd, message.c_str(),  message.length(), 0 );

        if ( send_ok == -1 )
        {
            int error = errno;
            throw InternetSocketException( error );
        }
    }

    int getDescriptor()
    {
        return fd;
    }
};

#endif