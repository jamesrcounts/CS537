#ifndef EDGE_TEST_H
#define EDGE_TEST_H 1

#include <igloo/igloo.h>
#include <vertex.h>
#include <edge.h>

using namespace igloo;

Context( Describe_An_Edge )
{
    Spec( It_Has_A_Cost_And_Two_Nodes )
    {
        Vertex a( "a" );
        Vertex b( "b" );
        Edge e( 3, a, b );
        Approvals::Verify( e.ToString() );
    }
};
#endif