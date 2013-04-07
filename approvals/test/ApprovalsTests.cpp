#include <igloo/igloo.h>
#include "../namers/IglooNamerFactory.h"
#include "../reporters/Reporter.h"
#include "../StringWriter.h"
#include "../FileApprover.h"

using namespace igloo;
using namespace std;

class ReporterFactory
{
private:
    static Reporter *reporter;
    ReporterFactory()  {}
    ~ReporterFactory() {}
public:
    static Reporter &GetCurrentReporter()
    {
        return *reporter;
    }

    template<typename ReporterType>
    static ReporterType &UseReporter( )
    {
        if ( reporter != NULL )
        {
            delete reporter;
        }

        ReporterType *t = new ReporterType();
        reporter = t;
        return *t;
    }
};

Reporter *ReporterFactory::reporter;

class Approvals
{
private:
    Approvals() {}
    ~Approvals() {}
public:
    static void Verify( std::string contents )
    {
        StringWriter writer( contents );
        Namer &namer = IglooNamerFactory::GetCurrentNamer();
        Reporter &reporter = ReporterFactory::GetCurrentReporter();
        FileApprover::verify( namer, writer, reporter );
    }
};

Context( DescribeTheVerifyMethod )
{
    Spec( ItVerifiesUsingConfiguredReporter )
    {
        stringstream expected;
        expected << "fake " <<
                 "/ApprovalsTests.DescribeTheVerifyMethod.ItVerifiesUsingConfiguredReporter.received.txt " <<
                 "/ApprovalsTests.DescribeTheVerifyMethod.ItVerifiesUsingConfiguredReporter.approved.txt ";
        TestReporter &reporter = ReporterFactory::UseReporter<TestReporter>();
        AssertThrows( ApprovalException, Approvals::Verify( "Hello" ) )
        string cmd = reporter.launcher.receivedCommand();
        string dir = IglooNamerFactory::TestDirectory();

        size_t at;

        do
        {
            at = cmd.find( dir );

            if ( at == string::npos )
            {
                break;
            }

            string b = cmd.substr( 0, at );
            string c = cmd.substr( at + dir.size() );
            cmd = b + c;
        }
        while ( at != string::npos );

        AssertThat( cmd, Equals( expected.str() ) );
        string received = IglooNamerFactory::GetCurrentNamer().getReceivedFile( ".txt" );
        remove( received.c_str() );
    }
};

int main( int argc, char const *argv[] )
{
    DefaultTestResultsOutput output;
    TestRunner runner( output );

    IglooMetadataListener listener;
    runner.AddListener( &listener );
    MinimalProgressTestListener mpl;
    runner.AddListener( &mpl );

    return runner.Run();
}
