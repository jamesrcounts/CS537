#include <algorithm>
#include <string>
#include <vector>
#include <queue>
#include <iostream>
#include "vertex.h"
#include "edge.h"
#include "graph.h"

using namespace std;

int main( int argc, char const *argv[] )
{
    std::vector<Vertex> nodes;
    nodes.push_back( Vertex( "a" ) );
    nodes.push_back( Vertex( "b" ) );
    nodes.push_back( Vertex( "c" ) );
    nodes.push_back( Vertex( "d" ) );
    nodes.push_back( Vertex( "e" ) );
    nodes.push_back( Vertex( "f" ) );
    nodes.push_back( Vertex( "g" ) );
    nodes.push_back( Vertex( "h" ) );
    nodes.push_back( Vertex( "i" ) );
    nodes.push_back( Vertex( "j" ) );
    nodes.push_back( Vertex( "k" ) );
    nodes.push_back( Vertex( "l" ) );

    std::vector<Edge> edges;
    edges.push_back( Edge( 3, nodes[0], nodes[1] ) );
    edges.push_back( Edge( 5, nodes[0], nodes[2] ) );
    edges.push_back( Edge( 4, nodes[0], nodes[3] ) );
    edges.push_back( Edge( 3, nodes[1], nodes[4] ) );
    edges.push_back( Edge( 6, nodes[1], nodes[5] ) );
    edges.push_back( Edge( 2, nodes[2], nodes[3] ) );
    edges.push_back( Edge( 1, nodes[3], nodes[4] ) );
    edges.push_back( Edge( 2, nodes[4], nodes[5] ) );
    edges.push_back( Edge( 4, nodes[2], nodes[6] ) );
    edges.push_back( Edge( 5, nodes[3], nodes[7] ) );
    edges.push_back( Edge( 4, nodes[4], nodes[8] ) );
    edges.push_back( Edge( 5, nodes[5], nodes[9] ) );
    edges.push_back( Edge( 3, nodes[6], nodes[7] ) );
    edges.push_back( Edge( 6, nodes[7], nodes[8] ) );
    edges.push_back( Edge( 3, nodes[8], nodes[9] ) );
    edges.push_back( Edge( 6, nodes[6], nodes[10] ) );
    edges.push_back( Edge( 7, nodes[7], nodes[10] ) );
    edges.push_back( Edge( 5, nodes[8], nodes[11] ) );
    edges.push_back( Edge( 9, nodes[9], nodes[11] ) );
    edges.push_back( Edge( 8, nodes[10], nodes[11] ) );

    Graph g( nodes, edges );
    std::string from = nodes[0].Id;
    g.Dijkstra( from );
    return 0;
}
