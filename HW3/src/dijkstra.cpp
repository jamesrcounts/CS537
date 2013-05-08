#include <utility> // for pair
#include <algorithm>
#include <iterator>
#include "Parser.h"

using namespace std;

int main( int argc, char const *argv[] )
{
    int start = atoi( argv[1] );
    int end = atoi( argv[2] );
    string filename( argv[3] );

    Parser p( filename );
    Graph g = p.CreateGraph();

    g.ComputePaths( start );
    list<vertex_t> path = g.GetShortestPathTo( end );

    cout << "Distance from " << start << " to " << end << ": " <<
         g.DistanceTo( end ) << endl;
    cout << "Path : ";
    copy( path.begin(),
          path.end(),
          ostream_iterator<vertex_t>( cout, " " ) );
    cout << endl;

    return 0;
}
