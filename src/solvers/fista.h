#ifndef FISTA_H
#define FISTA_H

#include "../graph/graph.h"
#include "dssolver.h"

class Fista : public DSSolver{
private:
    
    int m_R; // number of edges with an edge in R
    int delta; // maximum vertex degree
    double alpha; // learning rate
    std::vector<double> a;
    std::vector<double> r;

    void initSol();
    void calcR(const std::vector<double> &a);
    int gradient(std::vector<int> &r_argmx) const;
    void descent(const std::vector<int> &r_argmx, const int &r_nmx, std::vector<double> &a_desc) const;
    void project(std::vector<double> &a_proj) const;
public:

    ~Fista() override;
    Fista(const Graph *G_, const VertexSet *R_, const VertexSet *A_);

    void fista();
    void findDS() override;
    void solve() override;
} ;

#endif