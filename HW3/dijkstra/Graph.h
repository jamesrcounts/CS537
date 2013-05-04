#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <map>
#include "Vertex.h"

class Graph
{
private:
    std::map<std::string, Vertex *> nodes;
public:
    void AddNode( Vertex *v );
    void MakeNeighbors( Vertex *a, Vertex *b, int cost );
    int ShortestPath( std::string from, std::string to );
    std::string ToString();
};

#endif