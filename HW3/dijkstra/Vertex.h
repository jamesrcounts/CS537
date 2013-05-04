#ifndef VERTEX_H
#define VERTEX_H

#include <string>
#include <sstream>
#include <vector>
#include <map>

class Vertex
{
private:
    std::string id;
    std::vector<Vertex *> neighbors;
    std::map<std::string, int> weight;
public:
    int Distance;
    Vertex *Parent;

    Vertex( std::string name );
    void AddNeighbor( Vertex *n, int cost );
    std::string Name();
    std::string ToString();
};
#endif