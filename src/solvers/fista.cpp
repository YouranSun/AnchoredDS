#include "fista.h"
#include "../tools/utils.hpp"
#include "../nrcore/nrcore.h"

#include <algorithm>
#include <cmath>
#include <cassert>
#include <numeric>
#include <tuple>
#include <queue>
#include<ext/pb_ds/priority_queue.hpp>

Fista::~Fista(){}

Fista::Fista(const Graph *G_, const VertexSet *R_, const VertexSet *A_, const Setting &setting_):
    DSSolver(G_, R_, A_, setting_) {
    m_R = 0;
    calcW();

    for (int u = 0; u < (G -> n); ++u) {
        long long res = 0;
        for (auto [v, e]: (G -> g)[u]) {
            res += (w[e >> 1] > 0);
        }
        res = res * 2 * ((!(A -> in)[u]) + 1);
        res = res * res;
        // eprintf("u = %d res = %d\n", u, res);
        m_R = m_R + res;
    }

    // delta = 0;
    // for (int u = 0; u < (G -> n); ++u) {
    //     delta = std::max(delta, (int)(G -> g)[u].size());
    // }

    // alpha = 0.25 / (((A -> size()) + 1) * sqrt(1ll * m_R * delta));
    // alpha = 1.0 / (8 * ((A -> size()) + 1) * sqrt(1ll * m_R * delta));
    // alpha = 0.25 / ((A -> size()) + 1);
    // eprintf("m_R = %d\n", m_R);
    alpha = 1.0 / sqrt(m_R);
    eprintf("FISTA\talpha = %lf\n", alpha);
}

void Fista::calcW(){
    w = std::vector<int>(G -> m, 0);
    for (size_t e = 0; e < (G -> m); ++e) {
        size_t u = (G -> edges)[e].first;
        size_t v = (G -> edges)[e].second;
        w[e] = (R -> in)[u] + (R -> in)[v];
        // m_R += w[e];
    }
}

void Fista::initSol() {
    a = std::vector<double>((G -> m) * 2, 0);
    r = std::vector<double>(G -> n, 0);

    // for (int e = 0; e < (G -> m); ++e) {
    //     a[e << 1] += w[e] * 0.5;
    //     a[e << 1 | 1] += w[e] * 0.5;
    // }

    NRCore *c = new NRCore(G, R, A);
    for (size_t e = 0; e < (G -> m); ++e) {
        size_t u = (G -> edges)[e].first;
        size_t v = (G -> edges)[e].second;
        w[e] = (R -> in)[u] + (R -> in)[v];
        // if ((c -> pos)[u] > (c -> pos[v])) {
        //     a[e << 1] = w[e];
        // }
        // else {
        //     a[e << 1 | 1] = w[e];
        // }
        a[e << 1] = a[e << 1 | 1] = w[e] * 0.5;
    }

    // for (int e = 0; e < (G -> m); ++e) eprintf("(%d %d) (%.3lf %.3lf)\n", (G -> edges)[e].first, (G -> edges)[e].second, a[e << 1], a[e << 1 | 1]);
    // for (int u = 0; u < (G -> n); ++u) eprintf("%.3lf ", r[u]); eputs("");
        // w[e] = (R -> in)[u] + (R -> in)[v];
        // if ((c -> pos)[u] > (c -> pos[v])) {
        //     a[e << 1] = w[e];
        // }
        // else {
        //     a[e << 1 | 1] = w[e];
        // }
        // a[e << 1] = a[e << 1 | 1] = w[e] * 0.5;
    // }

    // for (int e = 0; e < (G -> m); ++e) eprintf("(%d %d) (%.3lf %.3lf)\n", (G -> edges)[e].first, (G -> edges)[e].second, a[e << 1], a[e << 1 | 1]);
    // for (int u = 0; u < (G -> n); ++u) eprintf("%.3lf ", r[u]); eputs("");
    delete c;
}

void Fista::calcR(const std::vector<double> &a) {
    for (size_t u = 0; u < (G -> n); ++u) {
        r[u] = 0;
        for (auto [v, e]: (G -> g)[u]) {
            r[u] += a[e];
        }
    }
}

size_t Fista::subgradient(std::vector<size_t> &r_argmx) const {
    size_t r_nmx = 0;
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

void Fista::gradient(const double &g, const std::vector<double> &a,
    std::vector<double> &nabla,
    const std::vector<size_t> &r_argmx,
    const size_t &r_nmx) const {
    nabla = std::vector<double>((G -> m) * 2, 0);
    for (size_t u = 0; u < (G -> n); ++u) {
        double d = 2 * ((!(A -> in)[u])) * r[u];
        for (auto [v, e]: (G -> g)[u]) {
            nabla[e] += d;
        }
    }
    for (size_t i = 0; i < r_nmx; ++i) {
        size_t u = r_argmx[i];
        // eprintf("u = %d r = %lf\n", u, r[u]);
        double d = 2 * ((A -> size()) * g) * r[u];
        for (auto [v, e]: (G -> g)[u]) {
            // eprintf("a = %lf delta = %lf\n", a_desc[e], alpha_cur * d);
            nabla[e] += d;
        }
    }
    return;
}

double norm2(const std::vector<double> &v) {
    double ret = 0;
    for (auto x: v) ret += x * x;
    return ret;
}

double Fista::learningRate(const std::vector<double> &nabla) const {
    static double z = 0, eta = alpha * 100000, pow_omega = 1;
    const double beta = 0.999, smooth = 1e-8, omega = 0.95;
    pow_omega = 1.0 * pow_omega * omega;
    z = beta * z + (1 - beta) * norm2(nabla);
    // eprintf("z = %lf\n", z);
    eta = eta / (sqrt(z) + smooth) * pow_omega;
    // eprintf("eta = %lf\n", eta);
    // eta = std::max(eta, alpha);
    // eprintf("eta = %lf alpha = %lf\n", eta, alpha);
    return eta;

}

double Fista::learningRateAsynchronous() const {
    static double z = alpha * 1024;
    z = std::max(z * 0.8, alpha * 0.125);
    return z;
}

void Fista::descent(const std::vector<size_t> &r_argmx,
                    const size_t &r_nmx,
                    std::vector<double> &a_desc, int &t) const {
    double g = 1.0 / r_nmx;
    std::vector<double> nabla((G -> m) * 2, 0);
    gradient(g, a_desc, nabla, r_argmx, r_nmx);
    double alpha_cur = learningRate(nabla);
    // static double alpha_cur = alpha * 50;
    // alpha_cur = std::max(2.0 /(t + 2), alpha);
    // alpha_cur = std::max(alpha_cur * pow(1.0 / 50, 1.0 / (T * 0.5)), alpha);
    // eprintf("FISTA\talpha_cur = %lf\n", alpha_cur);
    double r_mx = r[r_argmx[0]];
    // a_desc = a;
    for (size_t i = 0; i < (G -> m) * 2; ++i) {
        a_desc[i] -= nabla[i] * alpha_cur;
    }
    return;
}

void Fista::descentAsynchronous(const std::vector<size_t> &r_argmx,
                            const size_t &r_nmx,
                            const std::vector<double> &x_old,
                            std::vector<double> &x_new,
                            std::vector<double> &y,
                            std::vector<double> &r,
                            const double &tk, const double &tk_new) {
    double g = 1.0 / r_nmx;
    double r_mx = r[r_argmx[0]];
    double alpha_cur = learningRateAsynchronous();
    std::vector<size_t> e_ord((G -> m));
    for (size_t e = 0; e < (G -> m); ++e) {
        e_ord[e] = e;
    }
    shuffle(e_ord.begin(), e_ord.end(), rnd);

    for (auto e: e_ord) {
        size_t u = (G -> edges)[e].first, v = (G -> edges)[e].second;
        double d1 = 2 * ((!(A -> in)[u]) + ((r[u] == r_mx) * (A -> size()) * g)) * r[u];
        double d2 = 2 * ((!(A -> in)[u]) + ((r[v] == r_mx) * (A -> size()) * g)) * r[v];
        double &p = x_new[e << 1], &q = x_new[e << 1 | 1];
        r[u] -= p, r[v] -= q;
        p -= d1 * alpha_cur;
        q -= d2 * alpha_cur;
        double d = p - q;
        if (d > w[e]) p = w[e], q = 0;
        else if (d < -w[e]) p = 0, q = w[e];
        else p = (w[e] + d) * 0.5, q = (w[e] - d) * 0.5;
        r[u] += p, r[v] += q;

        y[e << 1 | 0] = x_new[e << 1 | 0] + (x_new[e << 1 | 0] + x_old[e << 1 | 0] * (-1)) * (1.0 * (tk - 1) / tk_new) + (x_new[e << 1 | 0] + y[e << 1 | 0] * (-1)) * (1.0 * tk / tk_new);
        y[e << 1 | 1] = x_new[e << 1 | 1] + (x_new[e << 1 | 1] + x_old[e << 1 | 1] * (-1)) * (1.0 * (tk - 1) / tk_new) + (x_new[e << 1 | 1] + y[e << 1 | 1] * (-1)) * (1.0 * tk / tk_new);
    }
}

void Fista::project(std::vector<double> &a_proj) const {
    for (size_t e = 0; e < (G -> m); ++e) {
        double &x = a_proj[e << 1], &y = a_proj[e << 1 | 1], d = x - y;
        if (d > w[e]) x = w[e], y = 0;
        else if (d < -w[e]) x = 0, y = w[e];
        else x = (w[e] + d) * 0.5, y = (w[e] - d) * 0.5;
        // eprintf("x = %lf y = %lf w = %d\n", x, y, w[e]);
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
    for (size_t i = 0; i < ret.size(); ++i) ret[i] += oth[i];
    return ret;
}

void Fista::fista() {
    initSol();

    std::vector<size_t> r_argmx(G -> n, 0);

    double tk = 1, tk_new = 1;
    // std::vector<double> a_proj_old = a;
    // std::vector<double> a_proj_new = a;
    std::vector<double> x_old = a;
    std::vector<double> y = a;

    for (int t = 1; t <= setting.T; ++t) {
        // eprintf("t = %d\n", t);
        // eprintf("t = %d\n", t);
        a = y;
        calcR(a);

        // eprintf("t = %d\n", t);
        // double desired = 0.0;
        // double r_max = *max_element(r.begin(), r.end());
        // for (int u = 0; u < (G -> n); ++u) {
        //     if (!(A -> in)[u]) desired += 1.0 * r[u] * r[u];
        //     else desired += 1.0 * r_max * r_max;
        // }
        // eprintf("r_max = %lf\n", r_max);
        // eprintf("desired = %lf\n", desired);
        // for (int u = 0; u < (G -> n); ++u) {
        //     eprintf("(%d %lf) ", u, r[u]);
        // }
        // eputs("");

        size_t r_nmx = subgradient(r_argmx);

        // a_proj_new = a;
        // descent(r_argmx, r_nmx, a_proj_new);
        // project(a_proj_new);

        // a = a_proj_new;
        // a = (a_proj_new * (t * 2 + 1) + a_proj_old * (1 - t)) * (1.0 / (t + 2));
        // a_proj_old = a_proj_new;

        std::vector<double> x_new = y;
        // project(x_new);

        // tk_new = (1.0 + sqrt(1 + 4 * tk * tk)) / 2.0;
        tk = t, tk_new = t + 2;
        // y = x_new + (x_new + x_old * (-1)) * (1.0 * (tk - 1) / tk_new) + (x_new + y * (-1)) * (1.0 * tk / tk_new);

        if (setting.is_asynchronous) {
            descentAsynchronous(r_argmx, r_nmx, x_old, x_new, y, r, tk, tk_new);
        }
        else {
            // eprintf("t = %d\n", t);
            // for (auto e = 0; e < (G -> m); ++e) {
            //     eprintf("(%d %d) (x = %lf y = %lf) alpha = (%lf %lf) %d\n", (G -> edges[e]).first, (G -> edges[e]).second, (x_new[e << 1], x_new[e << 1 | 1]), (y[e << 1], y[e << 1 | 1]), w[e]);
            // }
            descent(r_argmx, r_nmx, x_new, t);
            project(x_new);
            // for (auto e = 0; e < ((G -> m) << 1); ++e) eprintf("%lf ", x_new[e]); eputs("");
            // y = x_new + (x_new + y * (-1)) * (1.0 * (tk - 1) / tk_new);
            // for (auto e = 0; e < ((G -> m) << 1); ++e) eprintf("%lf ", x_new[e]); eputs("");
            y = x_new + (x_new + x_old * (-1)) * (1.0 * (tk - 1) / tk_new) + (x_new + y * (-1)) * (1.0 * tk / tk_new);
            // for (auto e = 0; e < (G -> m); ++e) {
            //     eprintf("(%d %d) (x = %lf y = %lf) alpha = (%lf %lf) %d\n",
            //         (G -> edges[e]).first, (G -> edges[e]).second,
            //         x_new[e << 1], x_new[e << 1 | 1],
            //         y[e << 1], y[e << 1 | 1],
            //         w[e]);
            // }
        }
        
        x_old = x_new;
        tk = tk_new;

        a = x_old;
        // a = a_proj_new;
        if (pow(2, (int)log2(t)) == t) {
            calcR(a);
            findDS(t);
            if (setting.use_core) {
                extractGraph(ds.back().rho);
            }
            if (setting.use_core) {
                extractGraph(ds.back().rho);
            }
        }

        if (setting.is_exact && t % setting.T_exact == 0) {
            calcR(a);
            findDS(t);
            printf("t = %d rho = %lf err = %lf time = %ld\n", t, ds.back().rho, ds.back().err, clock() - setting.start_time);
            printf("t = %d rho = %lf err = %lf time = %ld\n", t, ds.back().rho, ds.back().err, clock() - setting.start_time);
            if (!checkStable<double>(r, a, ord, level, (int)ds.back().vset.size())) {
                // puts("not stable");
                // puts("not stable");
                continue;
            }
            // puts("is stable");
            if (isDensest(ds.back(), level)) {
                double vm = 0, rss = 0;
                process_mem_usage(vm, rss);
                printf("vm = %lf rss = %lf cnt_flow = %d time_flow = %lld\n", vm, rss, setting.cnt_flow, setting.time_flow);
                break;
            }
        }

        
        if (1.0 * (clock() - setting.start_time) / CLOCKS_PER_SEC >= 10000) break;
        if (!setting.is_exact && (t >= 128 && ds.back().err <= 1.001)) {
            // puts("is stable");
            if (isDensest(ds.back(), level)) {
                double vm = 0, rss = 0;
                process_mem_usage(vm, rss);
                printf("vm = %lf rss = %lf cnt_flow = %d time_flow = %lld\n", vm, rss, setting.cnt_flow, setting.time_flow);
                break;
            }
        }

        
        if (1.0 * (clock() - setting.start_time) / CLOCKS_PER_SEC >= 10000) break;
        if (!setting.is_exact && (t >= 128 && ds.back().err <= 1.001)) break;
    }
}

void Fista::fractionalPeeling(std::vector<size_t> &ord) const {
    std::vector<double> r_ = r;
    std::vector<int> vis((G -> n), 0);
    __gnu_pbds::priority_queue<std::pair<double, size_t>,
        std::greater<std::pair<double, size_t> > > que;

    std::vector<__gnu_pbds::priority_queue<std::pair<double, size_t>,
        std::greater<std::pair<double, size_t> > >::point_iterator> it(G -> n);

    for (size_t u = 0; u < (G -> n); ++u) {
        if (!(A -> in)[u]) {
            it[u] = que.push(std::make_pair(r_[u], u));
        }
    }

    for (size_t i = 0; i < (G -> n) - (A -> size()); ++i) {
        size_t u = que.top().second;
        que.pop();
        vis[u] = true;
        // eprintf("i = %d u = %d\n", i, u);
        ord[i] = u;
        
        for (auto [v, e]: (G -> g)[u]) {
            if (!vis[v] && !(A -> in)[v]) {
                r_[v] -= a[e ^ 1];
                que.modify(it[v], std::make_pair(r_[v], v));
            }
        }
    }

    for (size_t i = 0; i < (A -> size()); ++i) {
        ord[(G -> n) - (A -> size()) + i] = (A -> list)[i];
    }
    reverse(ord.begin(), ord.end());
}

void Fista::findDS(int t) {
    ord.resize(G -> n);
    level.resize(G -> n);
    // eprintf("FINDDS\n");
    // for (int e = 0; e < (G -> m); ++e) eprintf("(%d %d) %d %d (%.3lf %.3lf)\n", (G -> edges)[e].first, (G -> edges)[e].second, (R -> in)[(G -> edges[e]).first], (R -> in)[(G -> edges[e]).second], a[e << 1], a[e << 1 | 1]);
    // eprintf("FINDDS\n");
    // for (int e = 0; e < (G -> m); ++e) eprintf("(%d %d) %d %d (%.3lf %.3lf)\n", (G -> edges)[e].first, (G -> edges)[e].second, (R -> in)[(G -> edges[e]).first], (R -> in)[(G -> edges[e]).second], a[e << 1], a[e << 1 | 1]);
    // for (int u = 0; u < (G -> n); ++u) eprintf("%.3lf ", r[u]); eputs("");

    double r_max = calcRMax(r);
   
    if (setting.is_fractional) {
        fractionalPeeling(ord);
    }
    else {
        for (size_t u = 0; u < (G -> n); ++u) ord[u] = u;
        std::sort(ord.begin(), ord.end(), [&](const int &u, const int &v) {
            if ((A -> in)[u] != (A -> in)[v]) {
                return (A -> in[u]) > (A -> in)[v];
            }
            return r[u] > r[v];
        });
    }

    // for (int u = 0; u < (G -> n); ++u) eprintf("%d ", (A -> in)[u]); eputs("");
    // for (int u = 0; u < (G -> n); ++u) eprintf("%d ", (A -> in)[u]); eputs("");
    // for (int u = 0; u < (G -> n); ++u) eprintf("%d ", ord[u]); eputs("");

    for(size_t i = 0; i < ord.size(); i++) level[ord[i]] = i;
    
    std::vector<size_t> rPava((G -> n), 0);
    for (int i = 0; i < (G -> m); ++i) {
        auto e = (G -> edges)[i];
        // eprintf("%d %d %d %d %d\n", e.first, e.second, (R -> in)[e.first], (R -> in)[e.second], w[i]);
        // eprintf("%d %d %d %d %d\n", e.first, e.second, (R -> in)[e.first], (R -> in)[e.second], w[i]);
        if(level[e.first] > level[e.second]) {
            rPava[level[e.first]] += w[i];
        }
        else {
            rPava[level[e.second]] += w[i];
        }
    }

    size_t cur_f = 0;
    best_rho = 0;
    best_g = 0;

    for (size_t i = 0; i < ord.size(); ++i) {
        // eprintf("u = %d %d rPava = %zu\n", ord[i], (R -> in)[ord[i]], rPava[i]);
        // eprintf("u = %d %d rPava = %zu\n", ord[i], (R -> in)[ord[i]], rPava[i]);
        cur_f += rPava[i];
    
        if ((double)cur_f / (i + 1) >= best_rho) {
            best_rho = (double)cur_f / (i + 1);
            best_g = i + 1;
            best_f = cur_f;
        }
    }

    err = r_max / best_rho;

    std::vector<size_t> cur_ds = std::vector<size_t>(best_g);
    for (size_t i = 0; i < best_g; ++i) {
        cur_ds[i] = ord[i];
    }
    ds.push_back(Solu(t, err, best_rho, cur_ds, clock() - setting.start_time));
    ds.push_back(Solu(t, err, best_rho, cur_ds, clock() - setting.start_time));
}

void Fista::extractGraph(const double &rho) {
    NRCore *C = new NRCore(G, R, A);
    C -> nrCore(G, R, A, rho);
    // eprintf("NRCORE\tnew graph size %d %d\n", (int)(G -> n), (int)(G -> m));
    delete C;
    calcW();
    // initSol();
    // initSol();
}

void Fista::solve() {
    if (setting.use_core || true) {
    if (setting.use_core || true) {
        NRCore *C = new NRCore(G, R, A);
        C -> nrCore(G, R, A, NRCore::USE_K_MAX);
        // eprintf("NRCORE\tnew graph size %d %d\n", (int)(G -> n), (int)(G -> m));

        write<Graph>(G, setting.path_G_persistent);
        write<VertexSet>(R, setting.path_R_persistent);
        write<VertexSet>(A, setting.path_A_persistent);

        delete C;
    }
    fista();
    // findDS();
}