#ifndef SERVER_H
#define SERVER_H

#include "Socket.h"

class Server : public Socket
{
private:
    int bind_result;
    int listen_result;
    int queue_size;

public:
    Server( string address, int options = 0 )
        :   bind_result( -1 ), listen_result( -1 ), queue_size( 4 )
    {
        parse_address( address );
        acquire_socket( options );
        bind_address();
        socket_listen();
    }

    ~Server()
    {
    }

    int accept_connection()
    {
        struct sockaddr_in cliaddr;
        socklen_t clilen = sizeof( cliaddr );
        int connfd = accept4( get_descriptor(),
                              ( struct sockaddr * ) &cliaddr,
                              &clilen,
                              SOCK_NONBLOCK );
        return connfd;
    }

    void bind_address()
    {
        if ( !is_bound() )
        {
            struct sockaddr_in address = get_address();
            bind_result = bind(
                              get_descriptor(),
                              ( struct sockaddr * )&address,
                              sizeof( address ) );
        }
    }

    bool is_bound()
    {
        return bind_result != -1;
    }

    bool is_listening()
    {
        return listen_result != -1;
    }

    void socket_listen()
    {
        if ( !is_listening() )
        {
            listen_result = listen(
                                get_descriptor(),
                                queue_size );
        }
    }

};

#endif