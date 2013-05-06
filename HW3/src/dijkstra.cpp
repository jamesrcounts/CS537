#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <list>

#include <limits> // for numeric_limits

#include <set>
#include <utility> // for pair
#include <algorithm>
#include <iterator>

using namespace std;

typedef int vertex_t;
typedef double weight_t;

const weight_t max_weight = std::numeric_limits<double>::infinity();

struct neighbor
{
    vertex_t target;
    weight_t weight;
    neighbor( vertex_t arg_target, weight_t arg_weight )
        : target( arg_target ), weight( arg_weight ) { }
};

typedef std::vector<std::vector<neighbor> > adjacency_list_t;


void DijkstraComputePaths( vertex_t source,
                           const adjacency_list_t &adjacency_list,
                           std::vector<weight_t> &min_distance,
                           std::vector<vertex_t> &previous )
{
    int n = adjacency_list.size();
    min_distance.clear();
    min_distance.resize( n, max_weight );
    min_distance[source] = 0;
    previous.clear();
    previous.resize( n, -1 );
    std::set<std::pair<weight_t, vertex_t> > vertex_queue;
    vertex_queue.insert( std::make_pair( min_distance[source], source ) );

    while ( !vertex_queue.empty() )
    {
        weight_t dist = vertex_queue.begin()->first;
        vertex_t u = vertex_queue.begin()->second;
        vertex_queue.erase( vertex_queue.begin() );

        // Visit each edge exiting u
        const std::vector<neighbor> &neighbors = adjacency_list[u];
        std::vector<neighbor>::const_iterator neighbor_iter = neighbors.begin();

        for ( ;
                neighbor_iter != neighbors.end();
                neighbor_iter++ )
        {
            vertex_t v = neighbor_iter->target;
            weight_t weight = neighbor_iter->weight;
            weight_t distance_through_u = dist + weight;

            if ( distance_through_u < min_distance[v] )
            {
                vertex_queue.erase( std::make_pair( min_distance[v], v ) );

                min_distance[v] = distance_through_u;
                previous[v] = u;
                vertex_queue.insert( std::make_pair( min_distance[v], v ) );

            }

        }
    }
}


std::list<vertex_t> DijkstraGetShortestPathTo(
    vertex_t vertex, const std::vector<vertex_t> &previous )
{
    std::list<vertex_t> path;

    for ( ; vertex != -1; vertex = previous[vertex] )
    {
        path.push_front( vertex );
    }

    return path;
}

enum ParseState { HEIGHT, WIDTH, GRAPH};
int main( int argc, char const *argv[] )
{
    int start = atoi( argv[1] );
    int end = atoi( argv[2] );
    std::string filename( argv[3] );

    int vx = 0;
    int hx;
    int cost = -1;
    int height;
    int width;
    ParseState state( HEIGHT );

    adjacency_list_t adjacency_list;
    std::istream_iterator<int> eos;

    std::ifstream infile( filename.c_str(), std::ifstream::in );

    char buffer[256];

    while ( infile.getline( buffer, 256 ) )
    {
        if ( buffer[0] == '\r' ||
                buffer[0] == '\n' ||
                buffer[0] == '#' )
        {
            continue;
        }

        stringstream line( buffer );
        memset( buffer, 0, 256 );

        switch ( state )
        {
        case HEIGHT:
            line >> height;
            state = WIDTH;
            break;
        case WIDTH:
            line >> width;

            if ( height != width )
            {
                cerr << "Height and Width do not match" << endl;
                exit( 255 );
            }

            adjacency_list = adjacency_list_t( width );
            state = GRAPH;
            break;
        case GRAPH:
            hx = 0;

            while ( line >> cost )
            {
                if ( vx != hx )
                {
                    adjacency_list[vx].push_back( neighbor( hx, cost ) );
                }

                ++hx;
            }

            ++vx;
            break;
        default:
            cerr << "Unknown Parsing State, buffer had: " << line.str() << endl;
        }
    }

    infile.close();

    std::vector<weight_t> min_distance;
    std::vector<vertex_t> previous;
    DijkstraComputePaths( start, adjacency_list, min_distance, previous );
    std::list<vertex_t> path = DijkstraGetShortestPathTo( end, previous );

    std::cout << "Distance from " << start << " to " << end << ": " <<
              min_distance[end] << std::endl;
    std::cout << "Path : ";
    std::copy( path.begin(),
               path.end(),
               std::ostream_iterator<vertex_t>( std::cout, " " ) );
    std::cout << std::endl;

    return 0;
}
