#include "Vertex.h"

using namespace std;

Vertex::Vertex( string name ) : id( name ), Distance( -1 ), Parent( NULL )
{

}

void Vertex::AddNeighbor( Vertex *n, int cost )
{
    neighbors.push_back( n );
    weight[n->Name()] = cost;
}

string Vertex::Name()
{
    return id;
}
string Vertex::ToString()
{
    stringstream t;
    t << id << endl;
    t << "Distance: ";

    if ( -1 == Distance )
    {
        t << "Infinity";
    }
    else
    {
        t << Distance;
    }

    t << "\nParent: ";
    t << ( Parent == NULL ? "NULL" : Parent->id ) << endl;

    t << "Neighbors: [";

    for ( std::vector<Vertex *>::iterator it = neighbors.begin();
            it != neighbors.end();
            ++it )
    {
        t << ( *it )->id << ", ";
    }

    t << "]" << endl;

    return t.str();
}