#ifndef NRCORE_H
#define NRCORE_H

#include "../graph/graph.h"

class NRCore{
private:
    Graph *&G;
    VertexSet *&R;
    VertexSet *&A;


    std::vector<int> w;
    std::vector<int> ct; // contribution of each vertex
    std::vector<int> cn; // coreness of each vertex
    std::vector<int> ord;
    std::vector<int> pos;

    int k_max = 0;
    VertexSet nrcore;

    void init();
    void bucketSort(std::vector<int> &beg_pos);
    void dec(int u, std::vector<int> &beg_pos, const int &head);
public:
    NRCore(Graph *&G_, VertexSet *&R_, VertexSet *&A_);

    void nrCore();
} ;

#endif