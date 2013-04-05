#ifndef NAMER_H
#define NAMER_H

#include <string>
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

class Namer
{
private:
    std::string approvalPath;
    std::string fileName;

    std::string createPath( std::string fileExtension, std::string fileType )
    {
        std::string name( fileName +
                          "." + fileType +
                          "." + eraseFirst( fileExtension, '.' ) );
        boost::filesystem::path p( approvalPath );
        p /= name;
        return p.string();
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

    std::string nameClean( std::string name )
    {
        return eraseLast( name, '.' );
    }

public:
    Namer( std::string path, std::string name )
        : approvalPath( path ), fileName( nameClean( name ) ) {}

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
