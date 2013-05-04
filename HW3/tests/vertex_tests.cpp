#include <igloo/igloo.h>
#include <approvals/Approvals.h>

#include "graphs.h"
#include "nodes.h"

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
