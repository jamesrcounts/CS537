#include <igloo/igloo.h>

using namespace igloo;
using namespace std;

class MeldReporter
{
private:
    std::string cmd;
public:
    MeldReporter() : cmd( "meld" ) {}
    std::string getCommand( std::string approved, std::string received )
    {
        return cmd + " " + received + " " + approved;
    }
};

Context( DescribeAMeldReporter )
{
    Spec( ItGeneratesASystemCommand )
    {
        MeldReporter m;
        string receivedFile = "r.txt";
        string approvedFile = "a.txt";
        string command = m.getCommand( approvedFile, receivedFile );
        Assert::That( command, Equals( "meld r.txt a.txt" ) );
    }
};

int main( int argc, char const *argv[] )
{
    return TestRunner::RunAllTests();
}
