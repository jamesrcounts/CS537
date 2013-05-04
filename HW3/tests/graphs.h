#include <igloo/igloo.h>
#include <approvals/Approvals.h>
#include <Graph.h>

using namespace igloo;

Context( Describe_A_Graph )
{
    Spec( An_Empty_Graph )
    {
        Graph g;
        Approvals::Verify( g.ToString() );
        Assert::That( g.ShortestPath( "1", "1" ), Equals( -1 ) );
    }

    Spec( A_Singleton )
    {
        Graph g;
        Vertex v( "1" );
        g.AddNode( &v );
        Approvals::Verify( g.ToString() );
        Assert::That( g.ShortestPath( "1", "1" ), Equals( 0 ) );
    }

    Spec( A_Pair )
    {
        Graph g;
        Vertex v( "1" );
        Vertex w( "2" );
        g.AddNode( &v );
        g.AddNode( &w );
        g.MakeNeighbors( &v, &w, 3 );
        Approvals::Verify( g.ToString() );
        Assert::That( g.ShortestPath( "1", "2" ), Equals( 3 ) );
        Assert::That( g.ShortestPath( "2", "1" ), Equals( 3 ) );
    }
};