#ifndef SOCKETCONNECTION_H
#define SOCKETCONNECTION_H

#include <arpa/inet.h>
#include <unistd.h>

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

    int getDescriptor()
    {
        return fd;
    }
};

#endif