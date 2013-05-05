#include "graph.h"

using namespace std;

struct AscendingCost
{
    bool operator()( const Vertex &a, const Vertex &b ) const
    {
        return a.Cost < b.Cost;
    }
    bool operator()( const Vertex *a, const Vertex *b ) const
    {
        return operator()( *a, *b );
    }
} byCost;

Vertex *Graph::FindByName( std::string name )
{
    Vertex *n = NULL;

    for ( std::vector<Vertex>::iterator i = nodes.begin();
            i != nodes.end();
            ++i )
    {
        if ( i->Id == name )
        {
            n = new Vertex( *i );
            break;
        }
    }

    return n;
}

void Graph::AddEdge( const Edge &e )
{
    edges.push_back( e );
}

void Graph::AddNode( const Vertex &v )
{
    nodes.push_back( v );
}

std::string Graph::GetPath( const Vertex &v ) const
{
    string result;

    for ( Vertex const *current = &v;
            current != NULL;
            current = current->GetParent() )
    {
        if ( current->GetParent() != NULL )
        {
            result = " - " + current->Id + result;
        }
        else
        {
            result = current->Id + result;
        }
    }

    return result;

}

void Graph::WritePathTo( const Vertex &v ) const
{
    cout << "to " << v.Id <<
         ": " << GetPath( v ) <<
         " of length " << v.Cost <<
         endl;
}

void Graph::WritePathTo( const Vertex *v ) const
{
    WritePathTo( *v );
}

vector<Vertex> Graph::Dijkstra( string from )
{
    Vertex *source = FindByName( from );

    if ( source == NULL )
    {
        std::vector<Vertex> t;
        return t;
    }

    source->Cost = 0;
    vector<Vertex > fringe;
    fringe.push_back( *source );

    vector<Vertex> treeNodes;
    vector<Vertex> remaining( nodes );

    for ( vx_iter i = nodes.begin();
            i != nodes.end();
            ++i )
    {
        std::sort( fringe.begin(), fringe.end(), byCost );
        vx_iter u = fringe.begin();
        Vertex x( *u );

        if ( x != *source )
        {
            WritePathTo( x );
        }

        treeNodes.push_back( x );
        vx_iter tn = find( remaining.begin(), remaining.end(), x );
        remaining.erase( tn );

        for ( vx_iter r = remaining.begin();
                r != remaining.end();
                ++r )
        {
            if ( r->IsAdjacentTo( x, edges ) )
            {
                int c = u->Cost + r->AdjacentEdge->Cost;

                if ( c < r->Cost )
                {
                    r->Cost = c;
                    r->Parent = &( x );
                }

                if ( find( fringe.begin(), fringe.end(), *r ) == fringe.end() )
                {
                    fringe.push_back( *r );
                }
            }
        }

        vx_iter fn = find( fringe.begin(), fringe.end(), x );
        fringe.erase( fn );
        cout << x.ToString() << "|";

        for ( std::vector<Vertex>::iterator f = fringe.begin();
                f != fringe.end();
                ++f )
        {
            std::cout << f->ToString();
        }
    }

    return treeNodes;
}

string Graph::ToString()
{
    stringstream t;
    t << "Nodes: [";

    for ( std::vector<Vertex>::iterator n = nodes.begin();
            n != nodes.end();
            ++n )
    {
        t << ( *n ).Id << ", ";
    }

    t << "]\nEdges:\n";

    for ( std::vector<Edge>::iterator e = edges.begin();
            e != edges.end();
            ++e )
    {
        t << ( *e ).ToString() << endl;
    }

    return t.str();
}