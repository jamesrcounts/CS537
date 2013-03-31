#ifndef STRINGWRITER_H
#define STRINGWRITER_H

#include <string>
#include <fstream>

class StringWriter
{
private:
    std::string s;
    std::string ext;

public:
    StringWriter( std::string contents, std::string fileExtension = "txt" )
        : s( contents ), ext( fileExtension ) {}

    std::string getFileExtension()
    {
        return ext;
    }

    void write( std::string path )
    {
        std::ofstream out( path.c_str(), std::ofstream::out );
        this->write( out );
        out.close();
    }

    void write( std::ostream &out )
    {
        out << s;
    }
};
#endif