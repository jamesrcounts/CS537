#ifndef INTERNETSOCKET_H
#define INTERNETSOCKET_H

#include <arpa/inet.h>
#include <errno.h>
#include <exception>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

class InternetSocketException
    : public exception
{
private:
    int e;
public:
    InternetSocketException( int error ) : e( error )
    {
    }

    ~InternetSocketException() throw() {};

    virtual const char *what() const throw()
    {
        return e == 0 ? "An unknown error occured" : strerror( e );
    }
};

class InternetSocket
{
private:
    int fd;
    struct sockaddr_in address;
protected:
    void create( int type )
    {
        fd = socket( AF_INET, type, 0 );

        if ( fd < 0 )
        {
            int error = errno;
            throw InternetSocketException( error );
        }

        int yes = 1;
        int opt_ok = setsockopt( fd,
                                 SOL_SOCKET,
                                 SO_REUSEADDR,
                                 &yes,
                                 sizeof( int ) );

        if ( opt_ok == -1 )
        {
            int error = errno;
            throw InternetSocketException( error );
        }
    }

public:

    InternetSocket( string address_spec = "",
                    uint16_t port = 1025 ) : fd( -1 )
    {
        bzero( &address, sizeof( sockaddr_in ) );

        address.sin_family = AF_INET;

        if ( address_spec == "" )
        {
            address_spec = "127.0.0.1";
        }

        unsigned long a;
        int ok = inet_pton( AF_INET,
                            address_spec.c_str(),
                            &a );

        if ( 0 < ok )
        {
            address.sin_addr.s_addr = a;
        }
        else
        {
            int error = ok == -1 ? errno : 0;
            throw InternetSocketException( error );
        }

        address.sin_port = htons( port );
    }

    ~InternetSocket()
    {
        close( fd );
    }


    struct sockaddr_in getAddress()
    {
        return address;
    }

    int getDescriptor()
    {
        return fd;
    }
};

#endif