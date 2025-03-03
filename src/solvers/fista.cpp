#include "fista.h"
#include "../tools/utils.hpp"

#include <algorithm>
#include <cmath>
#include <cassert>
#include <tuple>

Fista::~Fista(){}

Fista::Fista(const Graph *G_, const VertexSet *R_, const VertexSet *A_) : DSSolver(G_, R_, A_) {
    m_R = 0;
    w = std::vector<int>(G -> m);
    for (int e = 0; e < (G -> m); ++e) {
        int u = (G -> edges)[e].first;
        int v = (G -> edges)[e].second;
        w[e] = (R -> in)[u] + (R -> in)[v];
        m_R += w[e];
    }

    delta = 0;
    for (int u = 0; u < (G -> n); ++u) {
        delta = std::max(delta, (int)(G -> g)[u].size());
    }

    alpha = 0.25 / (((A -> size()) + 1) * sqrt(1ll * m_R * delta));
}

void Fista::initSol() {
    a = std::vector<double>((G -> m) * 2, 0);
    r = std::vector<double>(G -> n, 0);
    for (auto u: (R -> list)) {
        for (auto [v, e]: (G -> g)[u]) {
            a[e] += 1.0;
        }
    }
}

void Fista::calcR(const std::vector<double> &a) {
    for (int u = 0; u < (G -> n); ++u) {
        r[u] = 0;
        for (auto [v, e]: (G -> g)[u]) {
            r[u] += a[e];
        }
    }
}

int Fista::gradient(std::vector<int> &r_argmx) const {
    int r_nmx = 0;
    double r_mx = 0;
    for (int u = 0; u < (G -> n); ++u) {
        if (r[u] > r_mx) {
            r_mx = r[u];
            r_nmx = 0;
        }
        if (r[u] >= r_mx) {
            r_argmx[r_nmx++] = u;
        }
    }
    return r_nmx;
}

void Fista::descent(const std::vector<int> &r_argmx,
                    const int &r_nmx,
                    std::vector<double> &a_desc) const {
    double g = 1.0 / r_nmx;
    a_desc = a;
    for (int i = 0; i < r_nmx; ++i) {
        int u = r_argmx[i];
        for (auto [e, v]: (G -> g)[u]) {
            a_desc[e] -= alpha * g;
        }
    }
    return;
}

void Fista::project(std::vector<double> &a_proj) const {
    for (int e = 0; e < (G -> m); ++e) {
        double &x = a_proj[e << 1], &y = a_proj[e << 1 | 1], d = x - y;
        if (d > w[e]) x = w[e], y = 0;
        else if (d < -w[e]) x = 0, y = w[e];
        else x = (w[e] + d) * 0.5, y = (w[e] - d) * 0.5;
        assert(x >= 0 && y >= 0 && x + y == w[e]);
    }
}

std::vector<double> operator * (const std::vector<double> &ths, const double &x) {
    std::vector<double> ret = ths;
    for (auto y: ret) y *= x;
    return ret;
}

std::vector<double> operator + (const std::vector<double> &ths, const std::vector<double> &oth) {
    assert(ths.size() == oth.size());
    std::vector<double> ret = ths;
    for (int i = 0; i < ret.size(); ++i) ret[i] += oth[i];
    return ret;
}

void Fista::fista() {
    initSol();

    std::vector<int> r_argmx(G -> n, 0);

    std::vector<double> a_proj_old((G -> m) * 2, 0);
    std::vector<double> a_proj_new((G -> m) * 2, 0);

    for (int t = 1; t <= T; ++t) {
        // eprintf("t = %d\n", t);

        calcR(a);
        int r_nmx = gradient(r_argmx);

        descent(r_argmx, r_nmx, a_proj_new);
        project(a_proj_new);

        a = (a_proj_new * (t * 2 + 1) + a_proj_old * (1 - t)) * (1.0 / (t + 2));
        std::swap(a_proj_old, a_proj_new);
    }
    calcR(a_proj_new);
}

void Fista::findDS() {
    // for (int u = 0; u < (G -> n); ++u) eprintf("%.3lf ", r[u]); eputs("");
    std::vector<int> ord(G -> n);
    for (int u = 0; u < (G -> n); ++u) ord[u] = u;
    std::sort(ord.begin(), ord.end(), [&](const int &u, const int &v) {
        if ((A -> in)[u] != (A -> in)[v]) {
            return (A -> in[u]) > (A -> in)[v];
        }
        return r[u] > r[v];
    });
    // for (int u = 0; u < (G -> n); ++u) eprintf("%d ", ord[u]); eputs("");

    std::vector<int> level((G -> n), 0);
    for(int i = 0; i < ord.size(); i++) level[ord[i]] = i;
    
    std::vector<int> rPava((G -> n), 0);
    for (int i = 0; i < (G -> m); ++i) {
        auto e = (G -> edges)[i];
        if(level[e.first] > level[e.second]) {
            rPava[level[e.first]] += w[i];
        }
        else {
            rPava[level[e.second]] += w[i];
        }
    }
    
    int cur_f = 0;
    best_rho = 0;
    best_g = 0;

    for (int i = 0; i < ord.size(); ++i) {
        cur_f += rPava[i];
    
        if ((double)cur_f / (i + 1) >= best_rho) {
            best_rho = (double)cur_f / (i + 1);
            best_g = i + 1;
            best_f = cur_f;
        }
    }

    ds = std::vector<int>(best_g);
    for (int i = 0; i < best_g; ++i) {
        ds[i] = ord[i];
    }
}

void Fista::solve() {
    fista();
    findDS();
}