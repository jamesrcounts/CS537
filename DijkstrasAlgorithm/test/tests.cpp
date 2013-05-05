#include <igloo/igloo.h>
#include <approvals/Approvals.h>

#include "edge.test.h"
#include "vertex.test.h"
#include "graph.test.h"

using namespace igloo;

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
