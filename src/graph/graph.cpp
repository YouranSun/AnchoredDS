#include "graph.h"
#include "../tools/utils.hpp"

#include <random>
#include <algorithm>

std::map<int, int> Graph::init(int n_, int m_, std::vector<std::pair<int, int> > edges_) {

    n = n_, m = m_;
    edges = std::vector<std::pair<int, int> >(edges_.size());

    // std::vector<int> ver;
    // for (int e = 0; e < m; ++e) {
    //     auto [u, v] = edges_[e];
    //     ver.push_back(u);
    //     ver.push_back(v);
    // }
    // sort(ver.begin(), ver.end());
    // ver.erase(unique(ver.begin(), ver.end()), ver.end());
    std::map<int, int> ids;
    // for (int i = 0; i < n; ++i) {
    //     ids[ver[i]] = i;
    // }
    for (int e = 0; e < m; ++e) {
        auto [u, v] = edges_[e];
        if (ids.find(u) == ids.end()) {
            int tot = (int)ids.size();
            ids[u] = tot;
        }
        if (ids.find(v) == ids.end()) {
            int tot = (int)ids.size();
            ids[v] = tot;
        }
    }

    n_ = (int)ids.size();

    std::mt19937 rnd(2226701);
    shuffle(edges.begin(), edges.end(), rnd);
    
    g = std::vector<std::vector<std::pair<int, int> > > (n, std::vector<std::pair<int, int> >());
    for (int e = 0; e < m; ++e) {
        auto [u, v] = edges_[e];
        u = ids[u], v = ids[v];
        edges[e] = std::make_pair(u, v);
        g[u].push_back(std::make_pair(v, e << 1));
        g[v].push_back(std::make_pair(u, e << 1 | 1));
    }
    return ids;
}



void Graph::readFromText(const char *input_path) {
    FILE *input_file = fopen(input_path, "r");

    int n, m;
    fscanf(input_file, "%d%d", &n, &m);
    std::vector<std::pair<int, int> > edges(m);
    for (int e = 0; e < m; ++e) {
        int u, v; fscanf(input_file, "%d%d", &u, &v);
        edges[e] = std::make_pair(u, v);
    }
    fclose(input_file);
    
    init(n, m, edges);
}

void Graph::writeToText(const char *output_path) const {
    FILE *output_file = fopen(output_path, "w");

    fprintf(output_file, "%d %d\n", n, m);
    for (auto [u, v]: edges) {
        fprintf(output_file, "%d %d\n", u, v);
    }
    fclose(output_file);
}

Graph *Graph::induced_subgraph(const VertexSet &V, const std::vector<int> &mapping, VertexSet *R, VertexSet *A) const {
    Graph *G_ = new Graph();

    std::vector<std::pair<int, int> > edges_;
    for (auto e: edges) {
        if (V.in[e.first] && V.in[e.second]) {
            edges_.push_back(std::make_pair(mapping[e.first], mapping[e.second]));
        }
    }

    std::map<int, int> ord = (G_ -> init((int)V.list.size(), (int)edges_.size(), edges_));
    R -> reorder(ord);
    A -> reorder(ord);
    return G_;
}

void Graph::shrink(VertexSet *R, VertexSet *A) {
    std::vector<std::pair<int, int> > edges_;
    for (auto e: edges) {
        if ((R -> in)[e.first] || (R -> in)[e.second]) {
            edges_.push_back(e);
        }
    }
    std::map<int, int> ord = init(n, (int)(edges_.size()), edges);
    R -> reorder(ord);
    A -> reorder(ord);
}

void VertexSet::init(int n_, std::vector<int> list_) {
    n = n_;
    list = list_;
    in = std::vector<int>(n, 0);
    for (auto u: list) in[u] = true;
    eprintf("VERTEXSET size = %d\n", (int)size());
    // for (auto u: list) eprintf("%d ", u); eputs("");
}

size_t VertexSet::size() const{ return list.size(); }

VertexSet *VertexSet::induced_list(const VertexSet &V, const std::vector<int> &mapping) const {
    VertexSet *V_ = new VertexSet();
    std::vector<int> list_;
    for (auto u: list) {
        if (V.in[u]) {
            list_.push_back(mapping[u]);
        }
    }
    V_ -> init((int)(V.size()), list_);
    return V_;
}

void VertexSet::reorder(std::map<int, int> mapping) {
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

    int n, l;
    fscanf(input_file, "%d%d", &n, &l);
    std::vector<int> list(l);
    for (int i = 0; i < l; ++i) {
        int u; fscanf(input_file, "%d", &u);
        list[i] = u;
    }
    fclose(input_file);

    init(n, list);
}

void VertexSet::writeToText(const char *output_path) const {
    FILE *output_file = fopen(output_path, "w");

    fprintf(output_file, "%d %d\n", n, (int)size());
    for (auto u: list) {
        fprintf(output_file, "%d ", u);
    }
    fputs("", output_file);
    fclose(output_file);
}