#ifndef IGLOONAMERFACTORY_H
#define IGLOONAMERFACTORY_H

#include <string>
#include <igloo/igloo.h>
#include "Namer.h"

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

        std::string path( buf );
        return path;
    }

    static std::string FullTestName()
    {
        std::string name = TestName() + "." + ContextName() + "." + SpecName();
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
        std::string dir( ExePath() );
        unsigned slash = dir.find_last_of( "/" );

        if ( slash == std::string::npos )
        {
            return dir;
        }

        dir = dir.substr( 0, slash );
        return dir;
    }

    static std::string TestName()
    {
        std::string exe( ExePath() );
        unsigned slash = exe.find_last_of( "/" );

        if ( slash == std::string::npos )
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

class IglooMetadataListener : public igloo::TestListener
{
public:
    virtual void TestRunStarting() {}
    // cppcheck-suppress unusedFunction
    virtual void TestRunEnded( const igloo::TestResults &results ) {}
    // cppcheck-suppress unusedFunction
    virtual void ContextRunStarting( const igloo::ContextBase &context )
    {
        IglooNamerFactory::SetContextName( context.Name() );
    }
    // cppcheck-suppress unusedFunction
    virtual void ContextRunEnded( const igloo::ContextBase &context ) {}
    // cppcheck-suppress unusedFunction
    virtual void SpecRunStarting( const igloo::ContextBase &context,
                                  const std::string &specName )
    {
        IglooNamerFactory::SetSpecName( specName );
    }
    // cppcheck-suppress unusedFunction
    virtual void SpecSucceeded( const igloo::ContextBase &context,
                                const std::string &specName ) {}
    // cppcheck-suppress unusedFunction
    virtual void SpecFailed( const igloo::ContextBase &context,
                             const std::string &specName ) {}
};

#endif