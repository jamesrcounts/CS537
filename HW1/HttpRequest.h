#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#define MAXREAD 32768

class HttpRequest
{
public:
    HttpRequest();
    ~HttpRequest();

    static void Delete( int fd, string header )
    {
        vector<string> tokens = ParseHeader( header );
        WriteToStream( fd, HttpHandler::Delete( tokens[2], tokens[1] ) );
        close( fd );
    }

    static void Get( int fd, string header )
    {
        vector<string> tokens = ParseHeader( header );
        HttpHandler::Get( fd, tokens[2], tokens[1] );
        close( fd );
    }

    static void Head( int fd, string header )
    {
        vector<string> tokens = ParseHeader( header );
        WriteToStream( fd, HttpHandler::Head( tokens[2], tokens[1] ) );
        close( fd );
    }

    static	vector<string> ParseHeader( string header )
    {
        vector<string> tokens;
        stringstream header_stream( header );

        for ( int i = 0; i < 3; ++i )
        {
            string t;
            header_stream >> t ;
            tokens.push_back( t );
        }

        return tokens;
    }

    static void Put( int fd, string request, string header )
    {
        vector<string> tokens = ParseHeader( request );
        WriteToStream( fd,
                       HttpHandler::Put( tokens[2], header, tokens[1], fd ) );
        close( fd );
    }

    static string ReadHeader( int fd )
    {
        size_t idx = 0;
        char buffer[MAXREAD];
        bzero( buffer, MAXREAD );


        while ( idx < ( MAXREAD - 1 ) && 1 == read( fd, &buffer[idx], 1 ) )
        {
            if ( ( 0 < idx &&
                    buffer[idx] == '\n' &&
                    buffer[idx - 1] == '\n' ) ||
                    ( 3 < idx &&
                      ( buffer[idx] == '\n' && buffer[idx - 1] == '\r' ) &&
                      ( buffer[idx - 2] == '\n' && buffer[idx - 3] == '\r' ) ) )
            {
                break;
            }

            ++idx;
        }

        return string( buffer );
    }

    static string ReadRequest( int fd )
    {
        size_t idx = 0;
        size_t buffer_sz = 1024;
        char buffer[buffer_sz];
        bzero( buffer, buffer_sz );

        while ( idx < ( buffer_sz - 1 ) && 1 == read( fd, &buffer[idx], 1 ) )
        {
            if ( buffer[idx] == '\n' )
            {
                break;
            }

            ++idx;
        }

        return string( buffer );
    }

    static void WriteToStream( int fd, string message )
    {
        write( fd, message.c_str(), message.size() );
    }
};
#endif