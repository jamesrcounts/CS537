#include <igloo/igloo.h>
#include <approvals/Approvals.h>
#include <Vertex.h>

using namespace igloo;

Context( Describe_A_Vertex )
{
    Spec( It_Has_A_Name )
    {
        Vertex v( "MyVertex" );
        Approvals::Verify( v.ToString() );
    }

    Spec( It_Has_Neighbors )
    {
        Vertex v( "a" );
        Vertex b( "b" );
        Vertex c( "c" );
        v.AddNeighbor( &b, 1 );
        v.AddNeighbor( &c, 1 );
        Approvals::Verify( v.ToString() );
    }

    Spec( You_Can_Set_The_Distance )
    {
        Vertex v( "a" );
        v.Distance = 10;
        Approvals::Verify( v.ToString() );
    }

    Spec( You_Can_Set_The_Parent )
    {
        Vertex *p = new Vertex( "a" );
        Vertex v( "b" );
        v.Distance = 2;
        v.Parent = p;
        Approvals::Verify( v.ToString() );
        delete p;
    }
};