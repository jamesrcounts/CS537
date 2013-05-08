#include "Graph.h"

using namespace std;

Graph::Graph() : adjacency_list( 0 ) {};

Graph::Graph( const int &size ) : adjacency_list( size ) {};

void Graph::ComputePaths( vertex_t source )
{
    ResetPaths( source );
    set<pair<weight_t, vertex_t> > vertex_queue;
    vertex_queue.insert( make_pair( DistanceTo( source ), source ) );

    while ( !vertex_queue.empty() )
    {
        weight_t dist = vertex_queue.begin()->first;
        vertex_t u = vertex_queue.begin()->second;
        vertex_queue.erase( vertex_queue.begin() );

        // Visit each edge exiting u
        const vector<neighbor> &neighbors = Nodes()[u];
        vector<neighbor>::const_iterator neighbor_iter = neighbors.begin();

        for ( ;
                neighbor_iter != neighbors.end();
                neighbor_iter++ )
        {
            vertex_t v = neighbor_iter->target;
            weight_t weight = neighbor_iter->weight;
            weight_t distance_through_u = dist + weight;

            if ( distance_through_u < DistanceTo( v ) )
            {
                vertex_queue.erase( make_pair( DistanceTo( v ), v ) );

                UpdateDistanceTo( v, distance_through_u );
                UpdateParentOf( v, u );
                vertex_queue.insert( make_pair( DistanceTo( v ), v ) );

            }

        }
    }

}

weight_t Graph::DistanceTo( vertex_t v ) const
{
    return min_distance[v];
}

list<vertex_t> Graph::GetShortestPathTo( vertex_t vertex ) const
{
    list<vertex_t> path;

    for ( ; vertex != -1; vertex = Paths()[vertex] )
    {
        path.push_front( vertex );
    }

    return path;
}

const vector<vector<neighbor> >& Graph::Nodes() const
{
    return adjacency_list;
}

const vector<vertex_t> &Graph::Paths()const
{
    return previous;
}

void Graph::ResetPaths( vertex_t source )
{
    min_distance.clear();
    min_distance.resize( size(), max_weight );
    min_distance[source] = 0;
    previous.clear();
    previous.resize( size(), -1 );

}

void Graph::UpdateDistanceTo( vertex_t v, weight_t d )
{
    min_distance[v] = d;
}

void Graph::UpdateParentOf( vertex_t v, vertex_t u )
{
    previous[v] = u;
}

int Graph::size() const
{
    return adjacency_list.size();
}

vector<neighbor>& Graph::operator[]( const int &n )
{
    return adjacency_list[n];
}
