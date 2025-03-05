#ifndef DSSOLVER_H
#define DSSOLVER_H

#include "../graph/graph.h"

class DSSolver{
protected:
    const Graph *G;
    const VertexSet *R;
    const VertexSet *A;

    std::vector<int> w;
    std::vector<int> ds;

public:
    const int T = 64;

    int best_g;
    int best_f;
    double best_rho;

    DSSolver();
    virtual ~DSSolver();
    DSSolver(const Graph *G_, const VertexSet *R_, const VertexSet *A_);

    virtual void findDS() = 0;
    virtual void solve() = 0;
    std::vector<int> densestSubgraph() const;
} ;

#endif