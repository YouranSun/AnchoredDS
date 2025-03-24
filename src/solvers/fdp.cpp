#include "fdp.h"
#include "../tools/utils.hpp"

#include <algorithm>
#include <numeric>
#include <cmath>

FDP::~FDP(){}

FDP::FDP(const Graph *G_, const VertexSet *R_, const VertexSet *A_, const Setting &setting_):
    DSSolver(G_, R_, A_, setting_) {
    w = std::vector<int>(G -> m, 0);
    for (int e = 0; e < (G -> m); ++e) {
        int u = (G -> edges)[e].first;
        int v = (G -> edges)[e].second;
        w[e] = (R -> in)[u] + (R -> in)[v];
    }
}

void FDP::frankWolfe() { //run Frank-Wolfe
    std::vector<int> ord_e;
    for (int e = 0; e < (G -> m); ++e) if (w[e] > 0){
        ord_e.push_back(e);
    }
    shuffle(ord_e.begin(), ord_e.end(), rnd);
    r = std::vector<unsigned int>((G -> n) + 1);
    if (setting.is_exact) {
        a = std::vector<unsigned int>((G -> m) * 2 + 1);
    }
    for(auto & v : r) v = 0;

    for(unsigned int t = 1; t <= setting.T; t++) {
        unsigned int r_max = 0;
        // for (unsigned int i = 0; i < (G -> m); ++i) {
        for (auto i: ord_e) {
            auto e = (G -> edges)[i];
            if(r[e.first] < r[e.second]) {
                r_max = std::max(r_max, r[e.first] + w[i]);
                r[e.first] += w[i];
                if (setting.is_exact) a[i << 1] += w[i];
            }
            else if(r[e.first] > r[e.second]) {
                r_max = std::max(r_max, r[e.second] + w[i]);
                r[e.second] += w[i];
                if (setting.is_exact) a[i << 1 | 1] += w[i];
            }
            else {
                r_max = std::max(r_max, r[e.first] + (w[i] + 1) / 2);
                r[e.first] += (w[i] + 1) / 2;
                if (setting.is_exact) a[i << 1] += (w[i] + 1) / 2;
                r[e.second] += w[i] / 2;
                if (setting.is_exact) a[i << 1 | 1] += w[i] / 2;
            }
        }
        for(unsigned u : (A -> list)) {
            r[u] = r_max;
        }

        if (pow(2, (int)log2(t)) == t) {
            findDS(t);
        }

        if (setting.is_exact && t % setting.T_exact == 0) {
            findDS(t);
            printf("t = %d rho = %lf err = %lf\n", t, ds.back().rho, ds.back().err);
            if (!checkStable<unsigned int>(r, a, ord, level, (int)ds.back().vset.size())) continue;
            if (isDensest(ds.back(), level)) break;
        }
    }
}

void FDP::findDS(int t) {
    std::vector<double> r_real((G -> n));
    for (int i = 0; i < (G -> n); ++i) {
        r_real[i] = r[i] / t;
    }
    double r_max = calcRMax(r_real);

    //run tentative
    ord.resize((G -> n));
    for(unsigned int i = 0; i < (G -> n); i++) ord[i] = i;
    std::sort(ord.begin(), ord.end(), 
        [&](unsigned a, unsigned b) { return r[a] > r[b]; }
    );

    //sort vertices in A in the same level
    for(unsigned i = 0, j = 1; j < ord.size(); ) {
        while(j < ord.size() && r[ord[i]] == r[ord[j]]) j++;

        for(unsigned k = i, l = i; k < j; k++) {
            if(A -> in[ord[k]]) {
                std::swap(ord[l], ord[k]);
                l++;
            } 
        }

        i = j;
        j = j + 1;
    }

    std::vector<unsigned int> level((G -> n), __UINT32_MAX__);
    for(unsigned int i = 0; i < ord.size(); i++) level[ord[i]] = i;
    
    //rPava
    std::vector<unsigned> rPava((G -> n), 0);
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

    for (unsigned int i = 0; i < ord.size(); ++i) {
        cur_f += rPava[i];
    
        if ((double)cur_f / (i + 1) >= best_rho) {
            best_rho = (double)cur_f / (i + 1);
            best_g = i + 1;
            best_f = cur_f;
        }
    }

    err = r_max / best_rho;

    std::vector<int> cur_ds = std::vector<int>(best_g);
    for (int i = 0; i < best_g; ++i) {
        cur_ds[i] = ord[i];
    }

    ds.push_back(Solu(t, err, best_rho, cur_ds));
}

void FDP::solve() {
    frankWolfe();
}