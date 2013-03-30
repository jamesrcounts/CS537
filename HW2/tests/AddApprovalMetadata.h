#ifndef ADDAPPROVALMETADATA_H
#define ADDAPPROVALMETADATA_H

#include <igloo/igloo.h>

using namespace igloo;

class AddApprovalMetadata : public TestListener
{
public:
    virtual void TestRunStarting() {}
    // cppcheck-suppress unusedFunction
    virtual void TestRunEnded( const TestResults &results ) {}
    // cppcheck-suppress unusedFunction
    virtual void ContextRunStarting( const ContextBase &context ) {}
    // cppcheck-suppress unusedFunction
    virtual void ContextRunEnded( const ContextBase &context ) {}
    // cppcheck-suppress unusedFunction
    virtual void SpecRunStarting( const ContextBase &context,
                                  const std::string &specName )
    {
        context.SetAttribute( std::string( "SpecName" ), specName.c_str() );
    }
    // cppcheck-suppress unusedFunction
    virtual void SpecSucceeded( const ContextBase &context,
                                const std::string &specName ) {}
    // cppcheck-suppress unusedFunction
    virtual void SpecFailed( const ContextBase &context,
                             const std::string &specName ) {}
};

#endif