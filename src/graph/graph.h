#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <cstdio>
#include <map>

class VertexSet{
private:

public:
    size_t n;
    std::vector<size_t> list;
    std::vector<size_t> in;
   
    void init(size_t n_, std::vector<size_t> list_);

    void readFromText(const char *input_path);

    void writeToText(const char *output_path) const;

    VertexSet *induced_list(const VertexSet &V, const std::vector<size_t> &mapping) const;
    void reorder(std::map<size_t, size_t> mapping) ;

    size_t size() const;
} ;

class Graph {
private:

public:
    size_t n;
    size_t m;
    std::vector<std::vector<std::pair<size_t, size_t> > > g;
    std::vector<std::pair<size_t, size_t> > edges;

    std::map<size_t, size_t> init(size_t n_, size_t m_, std::vector<std::pair<size_t, size_t> > edges_);

    void shrink(VertexSet *R, VertexSet *A);

    std::map<size_t, size_t> readFromText(const char *input_path);

    void writeToText(const char *output_path) const;

    Graph *induced_subgraph(const VertexSet &V, const std::vector<size_t> &mapping, VertexSet *R, VertexSet *A) const;
} ;

#endif