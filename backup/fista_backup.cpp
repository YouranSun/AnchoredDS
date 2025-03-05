#include "fista.h"
#include "../tools/utils.hpp"
#include "../nrcore/nrcore.h"

#include <algorithm>
#include <cmath>
#include <cassert>
#include <tuple>
#include <queue>

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

    // for (int u = 0; u < (G -> n); ++u) {
    //     int res = 0;
    //     for (auto [v, e]: (G -> g)[u]) {
    //         res += (w[e >> 1] > 0);
    //     }
    //     res = res * 2 * ((!(A -> in)[u]) + 1);
    //     res = res * res;
    //     // eprintf("u = %d res = %d\n", u, res);
    //     m_R = m_R + res;
    // }

    delta = 0;
    for (int u = 0; u < (G -> n); ++u) {
        delta = std::max(delta, (int)(G -> g)[u].size());
    }

    alpha = 0.25 / (((A -> size()) + 1) * sqrt(1ll * m_R * delta));
    // alpha = 0.125 / (((A -> size()) + 1) * sqrt(1ll * m_R * delta));
    // alpha = 0.25 / ((A -> size()) + 1);
    // eprintf("m_R = %d\n", m_R);
    // alpha = 1.0 / (2 * sqrt(m_R));
}

void Fista::initSol() {
    a = std::vector<double>((G -> m) * 2, 0);
    r = std::vector<double>(G -> n, 0);

    // for (int e = 0; e < (G -> m); ++e) {
    //     a[e << 1] += w[e] * 0.5;
    //     a[e << 1 | 1] += w[e] * 0.5;
    // }

    // NRCore *c = new NRCore(G, R, A);
    // for (int e = 0; e < (G -> m); ++e) {
    //     int u = (G -> edges)[e].first;
    //     int v = (G -> edges)[e].second;
    //     if ((c -> pos)[u] > (c -> pos[v])) {
    //         a[e << 1] = w[e];
    //     }
    //     else {
    //         a[e << 1 | 1] = w[e];
    //     }
    // }

    // delete c;
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
    // eprintf("nmx = %d\n", r_nmx);
    // for (int i = 0; i < r_nmx; ++i) {
    //     eprintf("(%d %lf) ", r_argmx[i], r[r_argmx[i]]);
    // }
    // eputs("");
    return r_nmx;
}

void Fista::descent(const std::vector<int> &r_argmx,
                    const int &r_nmx,
                    std::vector<double> &a_desc) const {
    double r_mx = r[r_argmx[0]];
    double g = 1.0 / r_nmx;
    // a_desc = a;
    for (int u = 0; u < (G -> n); ++u) {
        double d = 2 * ((!(A -> in)[u]) + (r[u] == r_mx) * (int)(A -> size()) * g) * r[u];
        for (auto [v, e]: (G -> g)[u]) {
            a_desc[e] -= alpha * d;
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
        // eprintf("%lf %lf %d\n", x, y, w[e]);
        assert(x >= 0 && y >= 0 && x + y == w[e]);
    }
}

std::vector<double> operator * (const std::vector<double> &ths, const double &x) {
    std::vector<double> ret = ths;
    for (auto &y: ret) y *= x;
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

    double tk = 1, tk_new = 1;
    std::vector<double> a_proj_old = a;
    std::vector<double> a_proj_new = a;
    // std::vector<double> x_old = a;
    // std::vector<double> y = a;

    for (int t = 1; t <= T; ++t) {
        calcR(a);

        // eprintf("t = %d\n", t);
        // for (int u = 0; u < (G -> n); ++u) {
        //     eprintf("(%d %lf) ", u, r[u]);
        // }
        // eputs("");

        int r_nmx = gradient(r_argmx);

        a_proj_new = a;
        descent(r_argmx, r_nmx, a_proj_new);
        project(a_proj_new);

        a = a_proj_new;
        a = (a_proj_new * (t * 2 + 1) + a_proj_old * (1 - t)) * (1.0 / (t + 2));
        a_proj_old = a_proj_new;

        // std::vector<double> x_new = y;
        // descent(r_argmx, r_nmx, x_new);
        // project(x_new);

        // tk_new = (1.0 + sqrt(1 + 4 * tk * tk)) / 2.0;
        // y = x_new + (x_new + x_old * (-1)) * (1.0 * (tk - 1) / tk_new) + (x_new + y * (-1)) * (1.0 * tk / tk_new);
        // x_old = x_new;
        // tk = tk_new;
    }
    // calcR(x_old);
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