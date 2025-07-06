#include "fw.h"
#include "../tools/utils.hpp"

#include <algorithm>
#include <numeric>
#include <cmath>

FW::~FW(){}

FW::FW(const Graph *G_, const VertexSet *R_, const VertexSet *A_, const Setting &setting_):
    DSSolver(G_, R_, A_, setting_) {
    w = std::vector<int>(G -> m, 0);
    for (size_t e = 0; e < (G -> m); ++e) {
        size_t u = (G -> edges)[e].first;
        size_t v = (G -> edges)[e].second;
        w[e] = (R -> in)[u] + (R -> in)[v];
    }
}

void FW::frankWolfe() { //run Frank-Wolfe
    std::vector<size_t> ord_e;
    for (size_t e = 0; e < (G -> m); ++e) if (w[e] > 0){
        ord_e.push_back(e);
    }
    shuffle(ord_e.begin(), ord_e.end(), rnd);
    r = std::vector<double>((G -> n) + 1);
    a = std::vector<double>((G -> m) * 2 + 1);
    for(auto & v : r) v = 0;

    for(unsigned int t = 1; t <= setting.T; t++) {
        double r_max = 0, learning_rate = 2.0 / (t + 2);
        // for (unsigned int i = 0; i < (G -> m); ++i) {
        for (size_t u= 0; u < (G -> n); ++u) r[u] *= (1 - learning_rate);

        for (size_t e = 0; e < (G -> m); ++e) {
            auto [u, v] = (G -> edges)[e];
            a[e << 1] *= (1 - learning_rate);
            a[e << 1 | 1] *= (1 - learning_rate);
            if (r[u] < r[v])
                a[e << 1] += learning_rate * w[e], r[u] += learning_rate * w[e];
            else
                a[e << 1 | 1] += learning_rate * w[e], r[v] += learning_rate * w[e];
        }

        for (size_t u = 0; u < (G -> n); ++u) {
            r_max = std::max(r_max, r[u]);
        }

        for(size_t u : (A -> list)) {
            r[u] = r_max;
        }

        if (pow(2, (int)log2(t)) == t) {
            findDS(t);
        }
        if (1.0 * (clock() - setting.start_time) / CLOCKS_PER_SEC >= 10000 || (t >= 128 && ds.back().err <= 1.001)) break;

        // if (setting.is_exact && t % setting.T_exact == 0) {
        //     findDS(t);
        //     printf("t = %d rho = %lf err = %lf time = %lld\n", t, ds.back().rho, ds.back().err, clock() - setting.start_time);
        //     if (!checkStable<size_t>(r, a, ord, level, ds.back().vset.size())) continue;
        //     if (isDensest(ds.back(), level)) {
        //         double vm = 0, rss = 0;
        //         process_mem_usage(vm, rss);
        //         printf("vm = %lf rss = %lf cnt_flow = %d time_flow = %lld\n", vm, rss, setting.cnt_flow, setting.time_flow);
        //         break;
        //     }
        // }
    }
}

void FW::findDS(int t) {
    std::vector<double> r_real((G -> n));
    for (size_t i = 0; i < (G -> n); ++i) {
        r_real[i] = r[i];
    }
    double r_max = calcRMax(r_real);

    //run tentative
    ord.resize((G -> n));
    for(size_t i = 0; i < (G -> n); i++) ord[i] = i;
    std::sort(ord.begin(), ord.end(), 
        [&](size_t a, size_t b) { return r[a] > r[b]; }
    );

    //sort vertices in A in the same level
    for(size_t i = 0, j = 1; j < ord.size(); ) {
        while(j < ord.size() && r[ord[i]] == r[ord[j]]) j++;

        for(size_t k = i, l = i; k < j; k++) {
            if(A -> in[ord[k]]) {
                std::swap(ord[l], ord[k]);
                l++;
            } 
        }

        i = j;
        j = j + 1;
    }

    std::vector<size_t> level((G -> n), __UINT32_MAX__);
    for(size_t i = 0; i < ord.size(); i++) level[ord[i]] = i;
    
    //rPava
    std::vector<unsigned> rPava((G -> n), 0);
    for (size_t i = 0; i < (G -> m); ++i) {
        auto e = (G -> edges)[i];
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
        cur_f += rPava[i];
    
        if ((double)cur_f / (i + 1) >= best_rho) {
            best_rho = (double)cur_f / (i + 1);
            best_g = i + 1;
            best_f = cur_f;
        }
    }

    err = r_max / best_rho;

    std::vector<size_t> cur_ds = std::vector<size_t>(best_g);
    for (int i = 0; i < best_g; ++i) {
        cur_ds[i] = ord[i];
    }

    ds.push_back(Solu(t, err, best_rho, cur_ds, clock() - setting.start_time));
}

void FW::solve() {
    frankWolfe();
}