#ifndef EDGE_H
#define EDGE_H 1

#include <algorithm>
#include <string>

class Vertex;

class Edge
{
private:
    Vertex *ends[2];
public:
    int Cost;
    Edge( int cost, Vertex &a, Vertex &b );
    Edge( const Edge &e );
    ~Edge();
    bool ConnectedTo( const Vertex &v ) const;
    std::string ToString();

};
#endif
