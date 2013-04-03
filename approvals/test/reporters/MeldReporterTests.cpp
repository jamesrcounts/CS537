#include <igloo/igloo.h>
#include "../../reporters/Reporter.h"
using namespace igloo;
using namespace std;

Context( DescribeAReporter )
{
    Spec( ItLaunchesTheSystemCommand )
    {
        TestReporter m;
        m.report( "r.txt", "a.txt" );
        Assert::That( m.launcher.receivedCommand(),
                      Equals( "fake r.txt a.txt " ) );
    }
};

int main( int argc, char const *argv[] )
{
    return TestRunner::RunAllTests();
}
