#include "edge.h"
#include "vertex.h"

using namespace std;

Edge::Edge( int cost, Vertex &a, Vertex &b ) : Cost( cost )
{
    ends[0] = &a;
    ends[1] = &b;
}

Edge::Edge( const Edge &e ) : Cost( e.Cost )
{
    ends[0] = e.ends[0];
    ends[1] = e.ends[1];
}

Edge::~Edge()
{
}

bool Edge::ConnectedTo( const Vertex &v ) const
{
    return find( ends, ends + 2, &v ) != ends + 2;
};

string Edge::ToString()
{
    stringstream t;
    t << Cost << " " << "(" << ends[0]->Id << ", " << ends[1]->Id << ")";
    return t.str();
}
