#include "dssolver.h"
#include "../tools/utils.hpp"
#include "../graph/HLPP.hpp"

Solu::Solu(){};

Solu::Solu(const int t_, const double err_, const double rho_, const std::vector<size_t> &vset_):
    t(t_), err(err_), rho(rho_), vset(vset_) {}

DSSolver::~DSSolver(){
    delete G;
    delete R;
    delete A;
};

DSSolver::DSSolver(){};

DSSolver::DSSolver(const Graph *G_, const VertexSet *R_, const VertexSet *A_, const Setting &setting_):
    G(new Graph(*G_)),
    R(new VertexSet(*R_)),
    A(new VertexSet(*A_)),
    setting(setting_),
    err(1e18),
    best_rho(0),
    ord(std::vector<size_t>(G -> n, 0)),
    level(std::vector<size_t>(G -> n, 0)){}


std::vector<size_t> DSSolver::densestSubgraph() const {
    return ds.back().vset;
}

void DSSolver::printResults(){
    for (size_t i = 0; i < ds.size(); ++i) {
        printf("t = %d density = %lf err = %lf\n", ds[i].t, ds[i].rho, ds[i].err);
        for (auto u: ds[i].vset) {
            printf("%d ", u);
        }
        puts("");
    }
}

double DSSolver::calcRMax(const std::vector<double> &r) const {
    // std::vector<int> ord((G -> n));
    // for (int u = 0; u < (G -> n); ++u) {
    //     ord[u] = u;
    // }
    // std::sort(ord.begin(), ord.end(), [&](const int &u, const int &v) {
    //     if ((A -> in)[u] != (A -> in)[v]) {
    //         return (A -> in[u]) > (A -> in)[v];
    //     }
    //     return r[u] > r[v];
    // });

    double sum = 0;
    double r_max = 0;
    bool flag = 0;
    
    double r_0 = *max_element(r.begin(), r.end());

    for (size_t i = 0; i < (G -> n); ++i) {
        size_t u = ord[i];
        sum += r[u];
        if (1.0 * (i + 1) * i > sum) {
            flag = true;
        }
        if (!flag) { // (binom(i, 2) * 2) / i
            r_max = std::max(r_max, 1.0 * (i + 1));
        }
        else {
            r_max = std::max(r_max, sum / (i + 1));
        }
        // eprintf("i = %d sum = %lf flag = %d\n", i, sum, flag);
    }

    // eprintf("r_max = %lf\n", r_max);
    return r_max;
}

bool DSSolver::isDensest(const Solu &sol, const std::vector<size_t> &pos) {
    typedef HLPP_noGlobal Network;
    static std::vector<size_t> ids((G -> n));
    size_t v_tot = sol.vset.size() + 2, e_tot = 0;

    for(auto [u, v] : (G -> edges)) {
        if(pos[u] < (int)(sol.vset.size()) && pos[v] < (sol.vset.size())) {
            ++e_tot;
        }
    }
    e_tot = (sol.vset.size()) * 2 + e_tot * 2;

    for(size_t i = 0; i < sol.vset.size(); i++) {
        ids[sol.vset[i]] = i;
    }

    std::vector<int> degR(G -> n);
    for(auto [u, v]: (G -> edges)) {
        if ((R -> in)[u] && pos[v] < (sol.vset.size())) {
            ++degR[ids[v]];
        }
        if ((R -> in)[v] && pos[v] < (sol.vset.size())) {
            ++degR[ids[u]];
        }
    }

    Network::Vertex s = sol.vset.size(), t = sol.vset.size() + 1;
    Network network(v_tot, e_tot, s, t);
    
    const double INFOFA = 1e10;
    for (auto u: sol.vset) {
        if((A -> in)[u]) {
            network.addEdge(s, ids[u], INFOFA);
        }
        else {
            network.addEdge(s, ids[u], degR[ids[u]]);
        }
    }

    for (size_t e = 0; e < (G -> edges).size(); ++e) {
        auto [u, v] = (G -> edges)[e];
        if((R -> in)[u]) {
            network.addEdge(ids[v], ids[u], w[e]);
        }
        if ((R -> in)[v]) {
            network.addEdge(ids[u], ids[v], w[e]);
        }
        
    }

    for (auto u: sol.vset) {
        network.addEdge(ids[u], t, sol.rho);
    }

    double flow = network.maxFlow();
    return fabs(flow - sol.rho * (sol.vset.size())) <=  1e-5;
}