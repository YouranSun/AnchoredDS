#include "dssolver.h"

DSSolver::~DSSolver(){};

DSSolver::DSSolver(){};

DSSolver::DSSolver(const Graph *G_, const VertexSet *R_, const VertexSet *A_): G(G_), R(R_), A(A_) {}


std::vector<int> DSSolver::densestSubgraph() const {
    return ds;
}