#ifndef FISTA_H
#define FISTA_H

#include "../graph/graph.h"
#include "../nrcore/nrcore.h"
#include "dssolver.h"
#include <ctime>
#include <random>

class Fista : public DSSolver{
private:

    size_t m_R; // number of edges with an edge in R
    size_t delta; // maximum vertex degree
    double alpha; // learning rate
    std::vector<double> a;
    std::vector<double> r;

    void initSol();
    void calcW();
    void calcR(const std::vector<double> &a);
    size_t subgradient(std::vector<size_t> &r_argmx) const;
    double learningRate(const std::vector<double> &nabla) const;
    double learningRateAsynchronous() const;
    void gradient(const double &g, const std::vector<double> &a, std::vector<double> &nabla, const std::vector<size_t> &r_argmx, const size_t &r_nmx) const;
    void descent(const std::vector<size_t> &r_argmx, const size_t &r_nmx, std::vector<double> &a_desc, int &t) const;
    void descentAsynchronous(const std::vector<size_t> &r_argmx, const size_t &r_nmx, const std::vector<double> &x_old, std::vector<double> &x_new, std::vector<double> &y, std::vector<double> &r, const double &tk, const double &tk_new);
    void project(std::vector<double> &a_proj) const;
    void fractionalPeeling(std::vector<size_t> &ord) const;
    void extractGraph(const double &rho);

public:

    ~Fista() override;
    Fista(const Graph *G_, const VertexSet *R_, const VertexSet *A_, const Setting &setting_);

    void fista();
    void findDS(int t) override;
    void solve() override;
} ;

#endif