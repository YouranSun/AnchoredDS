#include <algorithm>

#include "nrcore.h"
#include "../tools/utils.hpp"

NRCore::NRCore(const Graph *G_, const VertexSet *R_, const VertexSet *A_): G(G_), R(R_), A(A_){
    cn = std::vector<size_t>((G -> n), 0);
    ct = std::vector<size_t>((G -> n), 0);
    ord = std::vector<size_t>((G -> n), 0);
    pos = std::vector<size_t>((G -> n), 0);
    w = std::vector<int> ((G -> m), 0);
}

void NRCore::bucketSort(std::vector<size_t> &beg_pos) {
    std::vector<size_t> cnt(*std::max_element(ct.begin(), ct.end()) + 1);
    for (size_t u = 0; u < (G -> n); ++u) {
        ord[u] = u;
        ++cnt[ct[u]];
    }
    for (size_t i = 1; i < (cnt.size()); ++i) {
        cnt[i] += cnt[i - 1];
    }
    for (size_t u = 0; u < (G -> n); ++u) {
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

void NRCore::dec(size_t u, std::vector<size_t> &beg_pos, const size_t &head) {
    size_t v = ord[beg_pos[ct[u]]];
    std::swap(ord[pos[u]], ord[beg_pos[ct[u]]]);
    std::swap(pos[u], pos[v]);
    if (beg_pos[ct[u]] == head) {
        beg_pos[ct[u] - 1] = head;
    }
    ++beg_pos[ct[u]];
    --ct[u];
}

void NRCore::initCore() { // to calculate the initial contribution of each vertex and sort by bucket
    for (size_t e = 0; e < (G -> m); ++e) {
        size_t u = (G -> edges)[e].first, v = (G -> edges)[e].second;
        w[e] = ((R -> in)[u]) + ((R -> in)[v]);
    }
    
    for (size_t u = 0; u < (G -> n); ++u) {
        ct[u] = 0;
        for (auto [v, e]: (G -> g)[u]) {
            ct[u] += w[e >> 1];
        }
        // eprintf("u = %d ct = %d\n", u, ct[u]);
    }

    std::vector<size_t> beg_pos;
    std::vector<int> vis((G -> n), 0);
    bucketSort(beg_pos);

    for (size_t i = 0; i < (G -> n); ++i) {
        size_t u = ord[i];
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

void NRCore::nrCore(Graph *&G_save, VertexSet *&R_save, VertexSet *&A_save, double rho) {
    initCore();
    // for (auto u: ord) {
    //     eprintf("(%d %d) ", u, ct[u]);
    // }
    // eputs("");

    if (rho == USE_K_MAX) {
        for (size_t i = 0; i < (ord.size()); ++i) {
            // eprintf("(%d %d %d) ", ord[i], (size_t)(ord.size()) - i, ct[ord[i]]);
            k_max = std::max(k_max, std::min((ord.size()) - i, (size_t)ct[ord[i]]));
        }
        
        // eputs("");
        eprintf("NRCORE\tk_max = %d\n", k_max);
        rho = k_max;
    }

    // double k_anchored = (k_max * k_max * 0.5) / (k_max * 0.5 + (int)(A -> size()));

    std::vector<size_t> core_list;
    for (size_t i = 0; i < (size_t)(ord.size()); ++i) {
        if (ct[ord[i]] * 2 >= rho) {
            for (size_t j = i; j < (size_t)(ord.size()); ++j) {
                core_list.push_back(ord[j]);
            }
            for (auto u: (A -> list)) {
                if (pos[u] < i) {
                    core_list.push_back(u);
                }
            }
            break;
        }
    }

    // for (auto u: core_list) {
    //     eprintf("%d ", u);
    // }
    // eputs("");

    std::vector<size_t> mapping(G -> n);
    for (size_t i = 0; i < (size_t)(core_list.size()); ++i) {
        mapping[core_list[i]] = i;
    }

    nrcore.init(G -> n, core_list);

    Graph *G_old = G_save;
    VertexSet *R_old = R_save;
    VertexSet *A_old = A_save;
    R_save = R -> induced_list(nrcore, mapping);
    A_save = A -> induced_list(nrcore, mapping);
    G_save = G -> induced_subgraph(nrcore, mapping, R_save, A_save);
    
    // for (auto u: (R_save -> list)) printf("%d ", u); puts("");
    // for (auto u: (A_save -> list)) printf("%d ", u); puts("");
    // for (auto [u, v]: (G_save -> edges)) printf("(%d %d)\n", u, v); puts("");

    delete G_old;
    delete R_old;
    delete A_old;

    return;
}