#ifndef VERTEX_TEST_H
#define VERTEX_TEST_H 1

#include <igloo/igloo.h>
#include <vertex.h>

using namespace igloo;

Context( Describe_A_Vertex )
{
    Spec( It_Has_A_Name )
    {
        Vertex v( "a" );
        Approvals::Verify( v.ToString() );
    }
};
#endif