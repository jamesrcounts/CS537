#include "vertex.h"
#include "edge.h"

using namespace std;

Vertex::Vertex( std::string name ) :
    Cost( -1 ),
    Id( name ),
    AdjacentEdge( NULL ),
    Parent( NULL )
{
}

Vertex::Vertex( const Vertex &v ) :
    Cost( v.Cost ),
    Id( v.Id ),
    AdjacentEdge( NULL ),
    Parent( NULL )
{
    if ( v.AdjacentEdge != NULL )
    {
        AdjacentEdge = new Edge( *v.AdjacentEdge );
    }

    Parent = v.Parent;
}

Vertex::~Vertex()
{
    delete AdjacentEdge;
}

Vertex *Vertex::GetParent() const
{
    return Parent;
}

bool Vertex::IsAdjacentTo( const Vertex &v, std::vector<Edge>& edges )
{
    AdjacentEdge = NULL;

    for ( std::vector<Edge>::iterator e = edges.begin();
            e != edges.end();
            ++e )
    {
        if ( e->ConnectedTo( v ) && e->ConnectedTo( *this ) )
        {
            if ( AdjacentEdge == NULL || e->Cost < AdjacentEdge->Cost )
            {
                if ( AdjacentEdge != NULL )
                {
                    delete AdjacentEdge;
                }

                AdjacentEdge = new Edge( *e );
            }
        }
    }

    AdjacentEdge != NULL;
}

std::string Vertex::ToString()
{
    Vertex *parent = GetParent();
    string p = parent != NULL ? parent->Id : "-";
    string c;

    if ( -1 < Cost )
    {
        stringstream t;
        t << Cost;
        c = t.str();
    }
    else
    {
        c = "-";
    }

    return Id + "(" + p + ", " + c + ")";
}

Vertex &Vertex::operator=( const Vertex &v )
{
    if ( this == &v )
    {
        return *this;
    }

    int cost = v.Cost;
    string name = v.Id;
    Edge *e = NULL;
    Vertex *p = v.Parent;

    if ( v.AdjacentEdge != NULL )
    {
        e = new Edge( *v.AdjacentEdge );
    }

    delete AdjacentEdge;

    Cost = cost;
    Id = name;
    AdjacentEdge = e;
    Parent = p;
    return *this;
}

bool Vertex::operator==( const Vertex &v ) const
{
    if ( Id != v.Id )
    {
        return false;
    }

    if ( Cost != v.Cost )
    {
        return false;
    }

    return true;
}

bool Vertex::operator!=( const Vertex &v )const
{
    return !operator==( v );
}
