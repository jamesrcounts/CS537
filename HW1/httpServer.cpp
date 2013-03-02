#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <sstream>
#include <time.h>


#define MAXLINE	32768

using namespace std;

const int backlog = 4;

void serverHeader( int fd )
{
    string server( "Server: myhttpd/1.0\n" );
    write( fd, server.c_str(), server.size() );
}

void newLine( int fd )
{
    string newLine( "\n" );
    write( fd, newLine.c_str(), newLine.size() );
}

void lastModified( int fd, string path )
{
    struct stat statbuf;

    if ( stat( path.c_str(), &statbuf ) != -1 )
    {
        string buf( "Last-Modified: " );
        char tstr[80];

        struct tm *timeinfo = localtime( &statbuf.st_mtime );

        strftime( tstr, 80, "%c.", timeinfo );

        buf += tstr;
        buf += "\n";

        write( fd, buf.c_str(), buf.size() );
    }
}

void contentLength( int fd, string path )
{
    struct stat statbuf;

    if ( stat( path.c_str(), &statbuf ) != -1 )
    {
        stringstream buf;
        buf << "Content-Length: " << statbuf.st_size << endl;

        string response = buf.str();
        write( fd, response.c_str(), response.size() );
    }


}

void ok( int fd, string  protocol )
{
    string ok( protocol );
    ok += " 200 OK\n";
    write( fd, ok.c_str(), ok.size() );

}

void notFound( int fd, string protocol )
{
    string notfound( protocol );
    notfound += " 404 Not Found\n\n";
    write( fd, notfound.c_str(), notfound.size() );

}

void headerResponse( string protocol, string path, int fd )
{
    path = "." + path;

    if ( FILE *fp = fopen( path.c_str(), "r" ) )
    {
        fclose( fp );

        ok( fd, protocol );
        serverHeader( fd );
        lastModified( fd, path );
        contentLength( fd, path );
        newLine( fd );
    }
    else
    {
        notFound( fd, protocol );
    }
}

void fileResponse( string protocol,  string path, int fd )
{

    path = "." + path;

    headerResponse( protocol, path, fd );

    string contents;

    if ( FILE *fp = fopen( path.c_str(), "r" ) )
    {
        char buf[1024];

        while ( size_t len = fread( buf, 1, sizeof( buf ), fp ) )
        {
            contents += buf;
        }

        fclose( fp );

        write( fd, contents.c_str(), strlen( contents.c_str() ) );
    }
}

void httpHead( string request, int fd )
{
    stringstream request_stream( request );

    string parts[3];
    string p;

    int i = 0;

    while ( request_stream >> p )
    {
        parts[i] = p;
        ++i;
    }

    cout << "COMMAND: " << parts[0] << endl
         << "PATH: " << parts[1] << endl
         << "PROTOCOL: " << parts[2] << endl;

    headerResponse( parts[2], parts[1], fd );
    close( fd );

}

void httpGet( string request, int fd )
{
    stringstream request_stream( request );

    string parts[3];
    string p;

    int i = 0;

    while ( request_stream >> p )
    {
        parts[i] = p;
        ++i;
    }

    cout << "COMMAND: " << parts[0] << endl
         << "PATH: " << parts[1] << endl
         << "PROTOCOL: " << parts[2] << endl;

    fileResponse( parts[2], parts[1], fd );
    close( fd );

}

void *clientHandler( void *arg )
{
    char str[MAXLINE];

    int  fd = *( int * )( arg );
    bzero( str, MAXLINE );

    if ( read( fd, str, MAXLINE )  == 0 )
    {
        close( fd );
        return NULL;
    }

    cout << "Request: " << endl  << str << endl;
    char tstr[MAXLINE + 1];
    memcpy( tstr, str, MAXLINE );
    tstr[MAXLINE] = 0;

    stringstream request( tstr );

    string command;

    request >> command;
    request.seekg( 0 );
    request.getline( str, MAXLINE );
    string s( str );

    if ( command == "GET" )
    {
        httpGet( s, fd );
    }
    else if ( command == "HEAD" )
    {
        httpHead( s, fd );
    }
    else
    {
        cout << "Unknown COMMAND: " << command << endl;
        close( fd );
    }

    /*    for ( i = 0; i < strlen( str ); i++ )
        {
            cout << str[i] << endl;

            if ( str[i] >= 97 && str[i] <= 122 )
            {
                str[i] = ( 0 <= str[i] - 32 ) ? str[i] - 32 : 0 ;
            }
        }

        write( fd, str, strlen( str ) );
    */
    // close( fd );
}

int main( int argc, char *argv[] )
{

    int	listenfd, connfd;
    pthread_t tid;
    int     clilen;
    struct 	sockaddr_in cliaddr, servaddr;

    if ( argc != 3 )
    {
        printf( "Usage: httpServer <address> <port> \n" );
        return -1;
    }

    listenfd = socket( AF_INET, SOCK_STREAM, 0 );

    if ( listenfd == -1 )
    {
        fprintf( stderr,
                 "Error unable to create socket, errno = %d (%s) \n",
                 errno,
                 strerror( errno ) );
        return -1;
    }

    bzero( &servaddr, sizeof( servaddr ) );

    servaddr.sin_family 	   = AF_INET;
    servaddr.sin_addr.s_addr   = inet_addr( argv[1] );
    servaddr.sin_port          = htons( atoi( argv[2] ) );

    int bind_result = bind( listenfd,
                            ( struct sockaddr * )&servaddr,
                            sizeof( servaddr ) );

    if ( bind_result == -1 )
    {
        fprintf( stderr,
                 "Error binding to socket, errno = %d (%s) \n",
                 errno,
                 strerror( errno ) );
        return -1;

    }

    if ( listen( listenfd, backlog ) == -1 )
    {
        fprintf( stderr,
                 "Error listening for connection request, errno = %d (%s) \n",
                 errno,
                 strerror( errno ) );
        return -1;
    }

    while ( 1 )
    {
        clilen = sizeof( cliaddr );
        connfd = accept( listenfd,
                         ( struct sockaddr * )&cliaddr,
                         ( socklen_t * )&clilen );

        if ( connfd < 0 )
        {
            if ( errno == EINTR )
            {
                continue;
            }
            else
            {
                fprintf( stderr,
                         "Error connection request refused, errno = %d (%s) \n",
                         errno,
                         strerror( errno ) );
            }
        }

        int create_result = pthread_create( &tid,
                                            NULL,
                                            clientHandler,
                                            ( void * )&connfd );

        if ( create_result != 0 )
        {
            fprintf( stderr,
                     "Error unable to create thread, errno = %d (%s) \n",
                     errno,
                     strerror( errno ) );
        }

    }
}

