#ifndef VERTEX_H
#define VERTEX_H 1

#include <sstream>
#include <string>
#include <vector>

class Edge;

class Vertex
{
public:
    int Cost;
    std::string Id;
    Edge *AdjacentEdge;
    Vertex *Parent;

    Vertex( std::string name );
    Vertex( const Vertex &v );
    ~Vertex();

    Vertex *GetParent() const;
    bool IsAdjacentTo( const Vertex &v, std::vector<Edge> &edges );
    std::string ToString();
    bool operator==( const Vertex &v ) const;
    bool operator!=( const Vertex &v ) const;
    Vertex &operator=( const Vertex &v );
};

typedef std::vector<Vertex>::iterator vx_iter;
typedef std::vector<Vertex *>::iterator vxp_iter;

#endif
