#ifndef FILEAPPROVER_H
#define FILEAPPROVER_H

#include <fstream>
#include <sys/stat.h>
#include "ApprovalException.h"
#include "namers/Namer.h"
#include "StringWriter.h"
#include "reporters/Reporter.h"

class FileApprover
{
protected:
    FileApprover() {};
    ~FileApprover() {};

    static ApprovalException *verify( std::string receivedPath,
                                      std::string approvedPath )
    {
        int asize = fileSize( approvedPath );

        if ( -1 == asize )
        {
            return new ApprovalMissingException( receivedPath, approvedPath );
        }

        int rsize = fileSize( receivedPath );

        if ( -1 == rsize )
        {
            return new ApprovalMissingException( approvedPath, receivedPath );
        }

        if ( asize != rsize )
        {
            return new ApprovalMismatchException( receivedPath, approvedPath );
        }

        std::ifstream astream( approvedPath.c_str(),
                               std::ios::binary | std::ifstream::in );
        std::ifstream rstream( receivedPath.c_str(),
                               std::ios::binary | std::ifstream::in );

        while ( astream.good() && rstream.good() )
        {
            int a = astream.get();
            int r = rstream.get();

            if ( a != r )
            {
                return new ApprovalMismatchException( receivedPath, approvedPath );
            }
        }

        remove( receivedPath );
        return NULL;
    }

public:
    static bool fileExists( std::string path )
    {
        return fileSize( path ) != -1;
    }

    static int fileSize( std::string path )
    {
        struct stat statbuf;
        int stat_ok = stat( path.c_str(), &statbuf );

        if ( stat_ok == -1 )
        {
            return -1;
        }

        return int( statbuf.st_size );
    }

    static void verify( Namer n, StringWriter s, Reporter r )
    {
        std::string approvedPath = n.getApprovedFile( ".txt" );
        std::string receivedPath = n.getReceivedFile( ".txt" );
        s.write( receivedPath );
        ApprovalException *ae = verify( receivedPath, approvedPath );

        if ( ae != NULL )
        {
            r.report( receivedPath, approvedPath );
            ApprovalException e( *ae );
            delete ae;
            throw e;
        }
    }
};

#endif