#include "Parser.h"

using namespace std;

Parser::Parser( string filename ): file( filename ) {}

Graph Parser::CreateGraph()const
{
    int vx = 0;
    int hx;
    int cost = -1;
    int height;
    int width;
    ParseState state( HEIGHT );

    Graph graph;

    ifstream infile( file.c_str(), ifstream::in );

    char buffer[256];

    while ( infile.getline( buffer, 256 ) )
    {
        if ( buffer[0] == '\r' ||
                buffer[0] == '\n' ||
                buffer[0] == '#' )
        {
            continue;
        }

        stringstream line( buffer );
        memset( buffer, 0, 256 );

        switch ( state )
        {
        case HEIGHT:
            line >> height;
            state = WIDTH;
            break;
        case WIDTH:
            line >> width;

            if ( height != width )
            {
                cerr << "Height and Width do not match" << endl;
                throw 255;
            }

            graph = Graph( width );
            state = GRAPH;
            break;
        case GRAPH:
            hx = 0;

            while ( line >> cost )
            {
                if ( vx != hx )
                {
                    graph[vx].push_back( neighbor( hx, cost ) );
                }

                ++hx;
            }

            ++vx;
            break;
        default:
            cerr << "Unknown Parsing State, buffer had: " << line.str() << endl;
        }
    }

    infile.close();

    return graph;
}

