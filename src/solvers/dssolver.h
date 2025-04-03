#ifndef DSSOLVER_H
#define DSSOLVER_H

#include "../graph/graph.h"
#include "../run/setting.h"
#include <random>

class Solu{
public:
    int t;
    double err;
    double rho;
    std::vector<size_t> vset;
    Solu();
    Solu(int t_, const double err_, const double rho_, const std::vector<size_t> &vset_);
} ;

class DSSolver{
protected:
    Graph *G;
    VertexSet *R;
    VertexSet *A;

    std::vector<int> w;
    std::vector<size_t> ord;
    std::vector<size_t> level;
    std::vector<Solu> ds;

public:
    Setting setting;

    std::mt19937 rnd = std::mt19937(2226701);

    size_t best_g;
    size_t best_f;
    double err;
    double best_rho;

    DSSolver();
    virtual ~DSSolver();
    DSSolver(const Graph *G_, const VertexSet *R_, const VertexSet *A_, const Setting &setting_);

    virtual void findDS(int t) = 0;
    virtual void solve() = 0;
    std::vector<size_t> densestSubgraph() const;
    void printResults();

    double calcRMax(const std::vector<double> &r) const;
    bool isDensest(const Solu &sol, const std::vector<size_t> &pos);
    template<typename T> bool checkStable(const std::vector<T> &r, const std::vector<T> &a, const std::vector<size_t> &ord, const std::vector<size_t> pos, size_t ds_size);
} ;

template<typename T>
bool DSSolver::checkStable(const std::vector<T> &r, const std::vector<T> &a, const std::vector<size_t> &ord, const std::vector<size_t> pos, size_t ds_size) {
    std::vector<T> r2 = r;

    T r_max = *max_element(r.begin(), r.end());
    for (auto u: (A -> list)) {
        r2[u] = r_max;
    }

    T minRHatS = r2[ord[0]];
    T maxRRight = 0;

    for (size_t e = 0; e < (G -> m); ++e) {
        auto [u, v] = (G -> edges[e]);
        if(pos[u] < ds_size && pos[u] >= ds_size) {
            r2[u] -= a[e << 1];
            r2[v] += a[e << 1];
        }
        else if(pos[v] < ds_size && pos[u] >= ds_size) {
            r2[u] += a[e << 1 | 1];
            r2[v] -= a[e << 1 | 1];
        }
    }

    for(size_t i = 0; i < ds_size; i++) {
        minRHatS = std::min(minRHatS, r2[ord[i]]);
    }

    for(size_t i = ds_size; i < (G -> n); i++) {
        maxRRight = std::max(maxRRight, r2[ord[i]]);
    }

    printf("%lf %lf %lf\n", (double)minRHatS, (double)maxRRight, (double)r_max);

    if(minRHatS <= maxRRight) {
        return false;
    }
    else {
        return true;
    }
}

#endif