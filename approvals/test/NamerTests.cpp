#include <igloo/igloo.h>
#include <sstream>
#include "../Namer.h"

using namespace igloo;
using namespace std;

class IglooNamerFactory
{
private:
    Namer *c;
    static std::string currentContext;
    static std::string currentSpec;

    static std::string ExePath()
    {
        char buf[512];
        bzero( buf, 512 );
        ssize_t readlink_ok = readlink( "/proc/self/exe", buf, 512 );

        if ( readlink_ok == -1 )
        {
            return "";
        }

        string path( buf );
        return path;
    }

    static std::string FullTestName()
    {
        string name = TestName() + "." + ContextName() + "." + SpecName();
        return name;
    }

public:
    IglooNamerFactory() : c( NULL )
    {
    }

    ~IglooNamerFactory()
    {
        if ( c != NULL )
        {
            delete c;
        }
    }

    static std::string ContextName()
    {
        return currentContext;
    }

    static void SetContextName( std::string contextName )
    {
        currentContext = contextName;
    }

    static std::string SpecName()
    {
        return currentSpec;
    }

    static void SetSpecName( std::string specName )
    {
        currentSpec = specName;
    }

    static std::string TestDirectory()
    {
        string dir( ExePath() );
        unsigned slash = dir.find_last_of( "/" );

        if ( slash == string::npos )
        {
            return dir;
        }

        dir = dir.substr( 0, slash );
        return dir;
    }

    static std::string TestName()
    {
        string exe( ExePath() );
        unsigned slash = exe.find_last_of( "/" );

        if ( slash == string::npos )
        {
            return exe;
        }

        exe = exe.substr( slash + 1, exe.size() + 1 );
        return exe;
    }

    Namer &GetCurrentNamer()
    {
        if ( c != NULL )
        {
            delete c;
        }

        c = new Namer( TestDirectory(), FullTestName() );
        return *c;
    }
};

std::string IglooNamerFactory::currentContext;
std::string IglooNamerFactory::currentSpec;

class IglooMetadataListener : public TestListener
{
public:
    virtual void TestRunStarting() {}
    // cppcheck-suppress unusedFunction
    virtual void TestRunEnded( const TestResults &results ) {}
    // cppcheck-suppress unusedFunction
    virtual void ContextRunStarting( const ContextBase &context )
    {
        IglooNamerFactory::SetContextName( context.Name() );
    }
    // cppcheck-suppress unusedFunction
    virtual void ContextRunEnded( const ContextBase &context ) {}
    // cppcheck-suppress unusedFunction
    virtual void SpecRunStarting( const ContextBase &context,
                                  const std::string &specName )
    {
        IglooNamerFactory::SetSpecName( specName );
    }
    // cppcheck-suppress unusedFunction
    virtual void SpecSucceeded( const ContextBase &context,
                                const std::string &specName ) {}
    // cppcheck-suppress unusedFunction
    virtual void SpecFailed( const ContextBase &context,
                             const std::string &specName ) {}
};

Context( DescribeAnIglooNamerFactory )
{
    Spec( ItCanGiveYouTheSpecName )
    {
        Assert::That( IglooNamerFactory::SpecName(),
                      Equals( "ItCanGiveYouTheSpecName" ) );
    }

    Spec( ItCanGiveYouTheContextName )
    {
        Assert::That( IglooNamerFactory::ContextName(), Equals( Name() ) );
    }

    Spec( ItCanGiveYouTheTestName )
    {
        char buf[512];
        bzero( buf, 512 );
        ssize_t readlink_ok = readlink( "/proc/self/exe", buf, 512 );
        Assert::That( readlink_ok, IsGreaterThan( -1 ) );
        string exe( buf );
        unsigned slash = exe.find_last_of( "/" );
        exe = exe.substr( slash + 1, exe.size() + 1 );
        Assert::That( IglooNamerFactory::TestName(), Equals( exe ) );

    }

    Spec( ItCanGiveYouTheTestDirectory )
    {
        char buf[512];
        bzero( buf, 512 );
        ssize_t readlink_ok = readlink( "/proc/self/exe", buf, 512 );
        Assert::That( readlink_ok, IsGreaterThan( -1 ) );
        string dir( buf );
        unsigned slash = dir.find_last_of( "/" );
        dir = dir.substr( 0, slash );
        Assert::That( IglooNamerFactory::TestDirectory(), Equals( dir ) );
    }

    Spec( ItIncludesFileContextAndSpecNames )
    {
        stringstream expected;
        expected << IglooNamerFactory::TestDirectory() << "/NamerTests" <<
                 ".DescribeAnIglooNamerFactory" <<
                 ".ItIncludesFileContextAndSpecNames" <<
                 ".approved.txt";

        IglooNamerFactory in;
        Namer n = in.GetCurrentNamer();
        Assert::That( n.getApprovedFile( ".txt" ), Equals( expected.str() ) );
    }
};

Context( DescribeANamer )
{
    Context( Normalization )
    {
        Spec( ItCleansInput )
        {
            Assert::That( n->getApprovedFile( ".txt" ),
                          Equals( "/tmp/foo.cpp.approved.txt" ) );
        }

        // cppcheck-suppress unusedFunction
        void SetUp()
        {
            n = new Namer( "/tmp/", "foo.cpp." );
        }

        // cppcheck-suppress unusedFunction
        void TearDown()
        {
            delete n;
        }

        Namer *n;
    };

    Context( BasicFunctionality )
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
