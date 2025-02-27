#include <algorithm>

#include "nrcore.h"
#include "../tools/utils.hpp"

NRCore::NRCore(Graph *&G_, VertexSet *&R_, VertexSet *&A_): G(G_), R(R_), A(A_){
    cn = std::vector<int>((G -> n), 0);
    ct = std::vector<int>((G -> n), 0);
    ord = std::vector<int>((G -> n), 0);
    pos = std::vector<int>((G -> n), 0);
    w = std::vector<int> ((G -> m), 0);
}

void NRCore::bucketSort(std::vector<int> &beg_pos) {
    std::vector<int> cnt(*std::max_element(ct.begin(), ct.end()) + 1);
    for (int u = 0; u < (G -> n); ++u) {
        ord[u] = u;
        ++cnt[ct[u]];
    }
    for (int i = 1; i < (int)(cnt.size()); ++i) {
        cnt[i] += cnt[i - 1];
    }
    for (int u = 0; u < (G -> n); ++u) {
        ord[pos[u] = --cnt[ct[u]]] = u;
    }
    // for (auto u: ord) {
    //     eprintf("(%d %d) ", u, ct[u]);
    // }
    // eputs("");
    beg_pos = cnt;
    // for (auto p: beg_pos) {
    //     eprintf("%d ", p);
    // }
    // eputs("");
}

void NRCore::dec(int u, std::vector<int> &beg_pos, const int &head) {
    int v = ord[beg_pos[ct[u]]];
    std::swap(ord[pos[u]], ord[beg_pos[ct[u]]]);
    std::swap(pos[u], pos[v]);
    if (beg_pos[ct[u]] == head) {
        beg_pos[ct[u] - 1] = head;
    }
    ++beg_pos[ct[u]];
    --ct[u];
}

void NRCore::init() { // to calculate the initial contribution of each vertex and sort by bucket
    for (int e = 0; e < (G -> m); ++e) {
        int u = (G -> edges)[e].first, v = (G -> edges)[e].second;
        w[e] = ((R -> in)[u]) + ((R -> in)[v]);
    }
    
    for (int u = 0; u < (G -> n); ++u) {
        ct[u] = 0;
        for (auto [v, e]: (G -> g)[u]) {
            ct[u] += w[e >> 1];
        }
        // eprintf("u = %d ct = %d\n", u, ct[u]);
    }

    std::vector<int> beg_pos;
    std::vector<int> vis((G -> n), 0);
    bucketSort(beg_pos);

    for (int i = 0; i < (G -> n); ++i) {
        int u = ord[i];
        // cn[u] = ct[u];

        // delete vertex u
        ++beg_pos[ct[u]];
        vis[u] = true;


        for (auto [v, e]: (G -> g)[u]) if (!vis[v]) {
            for (int t = 0; t < w[e >> 1]; ++t) {
                dec(v, beg_pos, i + 1);
            }
        }

        // for (auto u: ord) {
        //     eprintf("(%d %d) ", u, ct[u]);
        // }
        // eputs("");
        // for (auto p: beg_pos) {
        //     eprintf("%d ", p);
        // }
        // eputs("");
    }
}

void NRCore::nrCore() {
    init();
    // for (auto u: ord) {
    //     eprintf("(%d %d) ", u, ct[u]);
    // }
    // eputs("");

    for (int i = 0; i < (int)(ord.size()); ++i) {
        // eprintf("(%d %d %d) ", ord[i], (int)(ord.size()) - i, ct[ord[i]]);
        k_max = std::max(k_max, std::min((int)(ord.size()) - i, ct[ord[i]]));
    }
    
    // eputs("");
    eprintf("NRCORE\tk_max = %d\n", k_max);

    std::vector<int> core_list;
    for (int i = 0; i < (int)(ord.size()); ++i) {
        if (ct[ord[i]] * 2 >= k_max) {
            for (int j = i; j < (int)(ord.size()); ++j) {
                core_list.push_back(ord[j]);
            }
            break;
        }
    }

    // for (auto u: core_list) {
    //     eprintf("%d ", u);
    // }
    // eputs("");

    std::vector<int> mapping(G -> n);
    for (int i = 0; i < (int)(core_list.size()); ++i) {
        mapping[core_list[i]] = i;
    }

    nrcore.init(G -> n, core_list);

    Graph *G_old = G; G = G -> induced_subgraph(nrcore, mapping); delete G_old;
    VertexSet *R_old = R; R = R -> induced_list(nrcore, mapping); delete R_old;
    VertexSet *A_old = A; A = A -> induced_list(nrcore, mapping); delete A_old;

    return;
}