#ifndef CLIENT_H
#define CLIENT_H 

#include <string.h>
#include "Socket.h"

class Client : public Socket
{
private:
    int connection_result;
    int written_bytes;
    int read_bytes;

    string server_response;
public:
    Client( string address )
        : connection_result( -1 ), written_bytes( 0 ), read_bytes( 0 )
    {
        parse_address( address );
        acquire_socket();
    }

    ~Client() {}

    int last_received()
    {
        return read_bytes;
    }

    string last_response()
    {
        return server_response;
    }

    int last_send()
    {
        return written_bytes;
    }

    void receive()
    {
        int buffer_size = 1024;
        char buffer[buffer_size];
        int result_size = 0;
        char *result = new char[result_size];

        int last_read = 0;

        do
        {
            last_read = read(
                            get_descriptor(),
                            ( void * )buffer,
                            buffer_size );

            if ( last_read == -1 )
            {
                break;
            }

            char *t = result;
            result = new char[result_size + last_read];
            memcpy( result, t, result_size );
            memcpy( result + result_size, buffer, last_read );
            result_size += last_read;

            delete[] t;
        }
        while ( 0 < last_read );

        read_bytes = result_size;
        server_response = string( result );
        delete[] result;
    }

    void send( string request )
    {
        written_bytes = write(
                            get_descriptor(),
                            ( void * )request.c_str(),
                            request.size() );
    }


    void socket_connect()
    {
        if ( !is_connected() )
        {
            struct sockaddr_in address = get_address();
            connection_result = connect(
                                    get_descriptor(),
                                    ( struct sockaddr * )&address,
                                    sizeof( address ) );
        }
    }

    bool is_connected()
    {
        return connection_result == 0;
    }
};

#endif