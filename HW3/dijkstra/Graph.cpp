#include "Graph.h"

using namespace std;

void Graph::AddNode( Vertex *v )
{
    nodes[v->Name()] = v;
}

void Graph::MakeNeighbors( Vertex *a, Vertex *b, int cost )
{
    nodes[a->Name()] = a;
    a->AddNeighbor( b, cost );
    nodes[b->Name()] = b;
    b->AddNeighbor( a, cost );
}


int Graph::ShortestPath( string from, string to )
{
    if ( nodes[from] == NULL || nodes[to] == NULL )
    {
        return -1;
    }

    nodes[from]->Distance = 0;
    return nodes[to]->Distance;
}

string Graph::ToString()
{
    stringstream t;
    t << "Nodes: [";

    for ( std::map<std::string, Vertex *>::iterator i = nodes.begin();
            i != nodes.end();
            ++i )
    {
        t << i->first << ", ";
    }

    t << "]\n";
    return t.str();
}