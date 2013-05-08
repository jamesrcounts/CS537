#ifndef GRAPH_H
#define GRAPH_H 1

#include <limits>
#include <list>
#include <set>
#include <vector>

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

class Graph
{
private:
    std::vector<std::vector<neighbor> > adjacency_list;
    std::vector<weight_t> min_distance;
    std::vector<vertex_t> previous;
public:
    Graph();
    Graph( const int &size );

    void ComputePaths( vertex_t source );
    weight_t DistanceTo( vertex_t v ) const;
    std::list<vertex_t> GetShortestPathTo( vertex_t vertex ) const;
    const std::vector<std::vector<neighbor> >& Nodes() const;
    const std::vector<vertex_t> &Paths()const;
    void ResetPaths( vertex_t source );
    void UpdateDistanceTo( vertex_t v, weight_t d );
    void UpdateParentOf( vertex_t v, vertex_t u );
    int size() const;
    std::vector<neighbor>& operator[]( const int &n );
};
#endif