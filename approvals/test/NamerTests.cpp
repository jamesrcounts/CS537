#include <igloo/igloo.h>
#include "../Namer.h"

using namespace igloo;
using namespace std;

Context( DescribeANamer )
{
    Spec( ItProvidesTheApprovedFilename )
    {
        Assert::That( n->getApprovedFile( "png" ),
                      Equals( "/tmp/foo.cpp.approved.png" ) );
    }

    Spec( ItProvidesTheReceivedFilename )
    {
        Assert::That( n->getReceivedFile( "txt" ),
                      Equals( "/tmp/foo.cpp.received.txt" ) );
    }

    void SetUp()
    {
        path = string( "/tmp" );
        name = string( "foo.cpp" );
        n = new Namer( path, name );
    }

    void TearDown()
    {
        delete n;
    }

    string name;
    string path;
    Namer *n;
};

int main( int argc, char const *argv[] )
{
    return TestRunner::RunAllTests();
}