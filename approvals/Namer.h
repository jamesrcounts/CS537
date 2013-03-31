#ifndef NAMER_H
#define NAMER_H

#include <string>
#include <iostream>

class Namer
{
private:
    std::string approvalPath;
    std::string fileName;

    std::string createPath( std::string fileExtension, std::string fileType )
    {
        return approvalPath +
               "/" +
               fileName +
               "." +
               fileType +
               "." +
               eraseFirst( fileExtension, '.' );
    }

    std::string eraseFirst( std::string s, char c )
    {
        std::string::iterator it = s.begin();

        if ( *it == c )
        {
            s.erase( it );
        }

        return s;
    }

    std::string eraseLast( std::string s, char c )
    {
        std::string::iterator it = s.end();
        --it;

        if ( *it == c )
        {
            s.erase( it );
        }

        return s;
    }

    std::string pathClean( std::string path )
    {
        return	eraseLast( path, '/' );
    }

    std::string nameClean( std::string name )
    {
        return eraseLast( name, '.' );
    }

public:
    Namer( std::string path, std::string name )
        : approvalPath( pathClean( path ) ), fileName( nameClean( name ) ) {}

    std::string getApprovedFile( std::string fileExtension )
    {
        return createPath( fileExtension, "approved" );
    }

    std::string getReceivedFile( std::string fileExtension )
    {
        return createPath( fileExtension, "received" );
    }
};


#endif
