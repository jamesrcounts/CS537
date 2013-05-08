#ifndef PARSER_H
#define PARSER_H 1

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "Graph.h"

enum ParseState { HEIGHT, WIDTH, GRAPH};

class Parser
{
private:
    std::string file;
public:
    Parser( std::string filename );
    Graph CreateGraph() const;
};
#endif