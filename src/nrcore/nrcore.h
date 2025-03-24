#ifndef NRCORE_H
#define NRCORE_H

#include "../graph/graph.h"

class NRCore{
private:
    const Graph *G;
    const VertexSet *R;
    const VertexSet *A;


    std::vector<int> w;
    std::vector<int> ct; // contribution of each vertex
    std::vector<int> cn; // coreness of each vertex

    int k_max = 0;
    VertexSet nrcore;

    void bucketSort(std::vector<int> &beg_pos);
    void dec(int u, std::vector<int> &beg_pos, const int &head);
public:
    static constexpr double USE_K_MAX = -1.0;

    std::vector<int> ord;
    std::vector<int> pos;
    NRCore(const Graph *G_, const VertexSet *R_, const VertexSet *A_);

    void initCore();
    void nrCore(Graph *&G_save, VertexSet *&R_save, VertexSet *&A_save, double rho);
} ;

#endif