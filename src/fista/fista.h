#ifndef FISTA_H
#define FISTA_H

#include "../graph/graph.h"

class Fista {
private:
    const Graph *G;
    const VertexSet *R;
    const VertexSet *A;
    std::vector<int> w;
    std::vector<int> ds;
    
    int m_R; // number of edges with an edge in R
    int delta; // maximum vertex degree
    double alpha; // learning rate
    std::vector<double> a;
    std::vector<double> r;

    const int T = 15;
    void initSol();
    void calcR(const std::vector<double> &a);
    int gradient(std::vector<int> &r_argmx) const;
    void descent(const std::vector<int> &r_argmx, const int &r_nmx, std::vector<double> &a_desc) const;
    void project(std::vector<double> &a_proj) const;
public:

    Fista(const Graph *G_, const VertexSet *R_, const VertexSet *A_);

    void fista();
    void findDS();
    void solve();
    std::vector<int> densestSubgraph() const;
} ;

#endif