#ifndef INTERNETSOCKET_H
#define INTERNETSOCKET_H

using namespace std;

class InternetSocketException
    : public exception
{
protected:
    string message;
public:
    InternetSocketException( int error, string prefix = "" )
    {
        string w( prefix );

        if ( error != 0 )
        {
            w += ": ";
            w += strerror( error );
        }

        message = w;
    }

    ~InternetSocketException() throw() {};

    virtual const char *what() const throw()
    {
        return message.c_str();
    }
};

class InvalidAddressException
    : public InternetSocketException
{
public:
    InvalidAddressException( int error )
        : InternetSocketException( error, "Tried to create a socket with an invalid address" )
    {};
};

class SocketException
    : public InternetSocketException
{
public:
    SocketException( int error )
        : InternetSocketException( error, "Could not create socket" )
    {};
};


class InternetSocket
{
protected:
    struct sockaddr_in address;
    int fd;

public:

    InternetSocket( string address_spec = "", uint16_t port = 1025 ) : fd( -1 )
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
            int error = errno;
            throw InvalidAddressException( error );
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