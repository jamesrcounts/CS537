#ifndef GRAPH_TEST_H
#define GRAPH_TEST_H 1

#include <igloo/igloo.h>
#include <vertex.h>
#include <edge.h>
#include <graph.h>

Context( Describe_A_Graph )
{
    Spec( It_Is_Made_Of_Nodes_And_Edges )
    {
        Vertex a( "a" );
        Vertex b( "b" );
        Edge c( 3, a, b );

        Graph g;
        g.AddNode( a );
        g.AddNode( b );
        g.AddEdge( c );

        Approvals::Verify( g.ToString() );
    }

    Spec( It_Can_Get_The_Path_To_A_Vertex )
    {
        Vertex a( "a" );
        Graph g;
        Approvals::Verify( g.GetPath( a ) );
    }


    Spec( It_Can_Find_A_Node_By_Name )
    {
        Vertex a( "a" );
        Vertex b( "b" );
        Edge c( 3, a, b );

        Graph g;
        g.AddNode( a );
        g.AddNode( b );
        g.AddEdge( c );

        Vertex *n = g.FindByName( "a" );

        Assert::That( *n, Equals( a ) );
        delete n;
    }

    Spec( Dijkstra_Is_Empty_When_Source_Not_Found )
    {
        Vertex a( "a" );
        Vertex b( "b" );
        Edge c( 3, a, b );

        Graph g;
        g.AddNode( a );
        g.AddNode( b );
        g.AddEdge( c );

        std::vector<Vertex> v;
        v = g.Dijkstra( "c" );
        Assert::That( v.size(), Equals( 0 ) );
    }

    Spec( Dijkstra_For_Singleton )
    {
        Vertex a( "a" );

        Graph g;
        g.AddNode( a );

        std::vector<Vertex> v;
        v = g.Dijkstra( "a" );
        Approvals::Verify( v[0].ToString() );
    }
    Spec( Dijkstra_For_Pair )
    {
        Vertex a( "a" );
        Vertex b( "b" );
        Edge c( 3, a, b );

        Graph g;
        g.AddNode( a );
        g.AddNode( b );
        g.AddEdge( c );

        std::vector<Vertex> v;
        v = g.Dijkstra( "a" );
        Approvals::Verify( v[0].ToString() );
    }
};

#endif