#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <cstdio>

class VertexSet{
private:

public:
    int n;
    std::vector<int> list;
    std::vector<int> in;
   
    void init(int n_, std::vector<int> list_);

    void readFromText(const char *input_path);

    VertexSet *induced_list(const VertexSet &V, const std::vector<int> &mapping) const;

    size_t size() const;
} ;

class Graph {
private:

public:
    int n;
    int m;
    std::vector<std::vector<std::pair<int, int> > > g;
    std::vector<std::pair<int, int> > edges;

    void init(int n_, int m_, std::vector<std::pair<int, int> > edges_);

    // To-do: check the 0-index or 1-index problem
    void readFromText(const char *input_path);

    Graph *induced_subgraph(const VertexSet &V, const std::vector<int> &mapping) const;
} ;

#endif