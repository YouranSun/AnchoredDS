#include "fdp.h"
#include "../tools/utils.hpp"

#include <algorithm>

FDP::~FDP(){}

FDP::FDP(const Graph *G_, const VertexSet *R_, const VertexSet *A_) : DSSolver(G_, R_, A_) {
    w = std::vector<int>(G -> m, 0);
    for (int e = 0; e < (G -> m); ++e) {
        int u = (G -> edges)[e].first;
        int v = (G -> edges)[e].second;
        w[e] = (R -> in)[u] + (R -> in)[v];
    }
}

void FDP::frankWolfe() { //run Frank-Wolfe
    r = std::vector<unsigned int>((G -> n) + 1);
    for(auto & v : r) v = 0;

    for(unsigned int t = 0; t < T; t++) {
        unsigned int r_max = 0;
        for (unsigned int i = 0; i < (G -> m); ++i) {
            auto e = (G -> edges)[i];
            if(r[e.first] < r[e.second]) {
                r_max = std::max(r_max, r[e.first] + w[i]);
                r[e.first] += w[i];
            }
            else if(r[e.first] > r[e.second]) {
                r_max = std::max(r_max, r[e.second] + w[i]);
                r[e.second] += w[i];
            }
            else {
                r_max = std::max(r_max, r[e.first] + (w[i] + 1) / 2);
                r[e.first] += (w[i] + 1) / 2;
                r[e.second] += w[i] / 2;
            }
        }
        for(unsigned u : (A -> list)) {
            r[u] = r_max;
        }
    }
}

void FDP::findDS() {
    //run tentative
    std::vector<unsigned int> reordered((G -> n));
    for(unsigned int i = 0; i < (G -> n); i++) reordered[i] = i;
    std::sort(reordered.begin(), reordered.end(), 
        [&](unsigned a, unsigned b) { return r[a] > r[b]; }
    );

    //sort vertices in A in the same level
    for(unsigned i = 0, j = 1; j < reordered.size(); ) {
        while(j < reordered.size() && r[reordered[i]] == r[reordered[j]]) j++;

        for(unsigned k = i, l = i; k < j; k++) {
            if(A -> in[reordered[k]]) {
                std::swap(reordered[l], reordered[k]);
                l++;
            } 
        }

        i = j;
        j = j + 1;
    }

    std::vector<unsigned int> level((G -> n), __UINT32_MAX__);
    for(unsigned int i = 0; i < reordered.size(); i++) level[reordered[i]] = i;
    
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

    for (unsigned int i = 0; i < reordered.size(); ++i) {
        cur_f += rPava[i];
    
        if ((double)cur_f / (i + 1) >= best_rho) {
            best_rho = (double)cur_f / (i + 1);
            best_g = i + 1;
            best_f = cur_f;
        }
    }

    ds = std::vector<int>(best_g);
    for (int i = 0; i < best_g; ++i) {
        ds[i] = reordered[i];
    }
}

void FDP::solve() {
    frankWolfe();
    findDS();
}