#ifndef GRAPH_H
#define GRAPH_H 1

#include <iostream>

#include "vertex.h"
#include "edge.h"

class Graph
{
private:
    std::vector<Vertex> nodes;
    std::vector<Edge> edges;
public:
    Graph() {};
    Graph( std::vector<Vertex> v, std::vector<Edge> e ) :
        nodes( v ),
        edges( e )
    {};
    void AddEdge( const Edge &e );
    void AddNode( const Vertex &v );
    std::vector<Vertex> Dijkstra( std::string from );
    Vertex *FindByName( std::string name );
    std::string GetPath( const Vertex &v ) const;
    std::string ToString();
    void WritePathTo( const Vertex &v ) const;
    void WritePathTo( const Vertex *v ) const;
};

#endif