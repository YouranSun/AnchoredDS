#include "graph.h"
#include "../tools/utils.hpp"

#include <random>
#include <algorithm>

std::map<size_t, size_t> Graph::init(size_t n_, size_t m_, std::vector<std::pair<size_t, size_t> > edges_) {

    n = n_, m = m_;
    edges = std::vector<std::pair<size_t, size_t> >(edges_.size());

    // std::vector<int> ver;
    // for (int e = 0; e < m; ++e) {
    //     auto [u, v] = edges_[e];
    //     ver.push_back(u);
    //     ver.push_back(v);
    // }
    // sort(ver.begin(), ver.end());
    // ver.erase(unique(ver.begin(), ver.end()), ver.end());
    std::map<size_t, size_t> ids;
    // for (int i = 0; i < n; ++i) {
    //     ids[ver[i]] = i;
    // }
    for (size_t e = 0; e < m; ++e) {
        auto [u, v] = edges_[e];
        if (ids.find(u) == ids.end()) {
            size_t tot = ids.size();
            ids[u] = tot;
        }
        if (ids.find(v) == ids.end()) {
            size_t tot = ids.size();
            ids[v] = tot;
        }
    }

    n_ = ids.size();

    std::mt19937 rnd(2226701);
    shuffle(edges.begin(), edges.end(), rnd);
    
    g = std::vector<std::vector<std::pair<size_t, size_t> > > (n, std::vector<std::pair<size_t, size_t> >());
    for (size_t e = 0; e < m; ++e) {
        auto [u, v] = edges_[e];
        // eprintf("u = %d v = %d ids = %d %d\n", u, v, ids[u], ids[v]);
        u = ids[u], v = ids[v];
        edges[e] = std::make_pair(u, v);
        g[u].push_back(std::make_pair(v, e << 1));
        g[v].push_back(std::make_pair(u, e << 1 | 1));
    }
    return ids;
}



std::map<size_t, size_t> Graph::readFromText(const char *input_path) {
    FILE *input_file = fopen(input_path, "r");

    size_t n, m;
    fscanf(input_file, "%zu%zu", &n, &m);
    std::vector<std::pair<size_t, size_t> > edges(m);
    for (size_t e = 0; e < m; ++e) {
        size_t u, v; fscanf(input_file, "%zu%zu", &u, &v);
        edges[e] = std::make_pair(u, v);
    }
    fclose(input_file);
    
    return init(n, m, edges);
}

void Graph::writeToText(const char *output_path) const {
    FILE *output_file = fopen(output_path, "w");

    fprintf(output_file, "%zu %zu\n", n, m);
    for (auto [u, v]: edges) {
        fprintf(output_file, "%zu %zu\n", u, v);
    }
    fclose(output_file);
}

Graph *Graph::induced_subgraph(const VertexSet &V, const std::vector<size_t> &mapping, VertexSet *R, VertexSet *A) const {
    Graph *G_ = new Graph();

    std::vector<std::pair<size_t, size_t> > edges_;
    for (auto e: edges) {
        if (V.in[e.first] && V.in[e.second]) {
            // eprintf("%d %d (%d %d)\n", e.first, e.second, mapping[e.first], mapping[e.second]);
            edges_.push_back(std::make_pair(mapping[e.first], mapping[e.second]));
        }
    }

    std::map<size_t, size_t> ord = (G_ -> init(V.list.size(), edges_.size(), edges_));
    for (auto u: (A -> list)) {
        if (ord.find(u) == ord.end()) {
            // eprintf("give %zu %zu\n", u, (G_ -> n));
            size_t tot = ord.size();
            ord[u] = tot;
        }
    }
    (G_ -> g).resize(G_ -> n);
    R -> reorder(ord);
    A -> reorder(ord);
    return G_;
}

void Graph::shrink(VertexSet *R, VertexSet *A) {
    std::vector<std::pair<size_t, size_t> > edges_;
    for (auto e: edges) {
        if ((R -> in)[e.first] || (R -> in)[e.second]) {
            edges_.push_back(e);
        }
    }
    std::map<size_t, size_t> ord = init(n, edges_.size(), edges);
    R -> reorder(ord);
    A -> reorder(ord);
}

void VertexSet::init(size_t n_, std::vector<size_t> list_) {
    n = n_;
    list = list_;
    in = std::vector<size_t>(n, 0);
    for (auto u: list) in[u] = true;
    // eprintf("VERTEXSET size = %zu\n", size());
    // for (auto u: list) eprintf("%d ", u); eputs("");
}

size_t VertexSet::size() const{ return list.size(); }

VertexSet *VertexSet::induced_list(const VertexSet &V, const std::vector<size_t> &mapping) const {
    VertexSet *V_ = new VertexSet();
    std::vector<size_t> list_;
    for (auto u: list) {
        // eprintf("u = %d in = %d mapping = %d\n", u, V.in[u], mapping[u]);
        if (V.in[u]) {
            list_.push_back(mapping[u]);
        }
    }
    V_ -> init(V.n, list_);
    return V_;
}

void VertexSet::reorder(std::map<size_t, size_t> mapping) {
    for (auto &u: list) {
        in[u] = false;
        u = mapping[u];
    }
    for (auto u: list) {
        in[u] = true;
    }
    return;
}

void VertexSet::readFromText(const char *input_path) {
    FILE *input_file = fopen(input_path, "r");

    size_t n, l;
    fscanf(input_file, "%zu%zu", &n, &l);
    std::vector<size_t> list(l);
    for (size_t i = 0; i < l; ++i) {
        size_t u; fscanf(input_file, "%zu", &u);
        list[i] = u;
    }
    fclose(input_file);

    init(n, list);
}

void VertexSet::writeToText(const char *output_path) const {
    FILE *output_file = fopen(output_path, "w");

    fprintf(output_file, "%zu %zu\n", n, size());
    for (auto u: list) {
        fprintf(output_file, "%zu ", u);
    }
    fputs("", output_file);
    fclose(output_file);
}