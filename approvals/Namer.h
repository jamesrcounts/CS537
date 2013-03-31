#ifndef NAMER_H
#define NAMER_H

#include <string>

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
               fileExtension;
    }

public:
    Namer( std::string path, std::string name )
        : approvalPath( path ), fileName( name ) {}
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