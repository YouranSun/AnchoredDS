#ifndef NRCORE_H
#define NRCORE_H

#include "../graph/graph.h"

class NRCore{
private:
    const Graph *G;
    const VertexSet *R;
    const VertexSet *A;


    std::vector<int> w;
    std::vector<size_t> ct; // contribution of each vertex
    std::vector<size_t> cn; // coreness of each vertex

    size_t k_max = 0;
    VertexSet nrcore;

    void bucketSort(std::vector<size_t> &beg_pos);
    void dec(size_t u, std::vector<size_t> &beg_pos, const size_t &head);
public:
    static constexpr double USE_K_MAX = -1.0;

    std::vector<size_t> ord;
    std::vector<size_t> pos;
    NRCore(const Graph *G_, const VertexSet *R_, const VertexSet *A_);

    void initCore();
    void nrCore(Graph *&G_save, VertexSet *&R_save, VertexSet *&A_save, double rho);
} ;

#endif