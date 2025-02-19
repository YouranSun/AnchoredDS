#include "graph.h"
#include "../tools/utils.hpp"

void Graph::init(int n_, int m_, std::vector<std::pair<int, int> > edges_) {
    n = n_, m = m_;
    edges = edges_;
    
    g.resize(n);
    for (int e = 0; e < m; ++e) {
        auto [u, v] = edges[e];
        g[u].push_back(std::make_pair(v, e << 1));
        g[v].push_back(std::make_pair(u, e << 1 | 1));
    }
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

    init(n, m, edges);
}

void VertexSet::init(int n_, std::vector<int> list_) {
    n = n_;
    list = list_;
    in = std::vector<int>(n, 0);
    for (auto u: list) in[u] = true;
    eprintf("size = %d\n", size());
    for (auto u: list) eprintf("%d ", u); eputs("");
}

size_t VertexSet::size() const{ return list.size(); }

void VertexSet::readFromText(const char *input_path) {
    FILE *input_file = fopen(input_path, "r");

    int n, l;
    fscanf(input_file, "%d%d", &n, &l);
    std::vector<int> list(l);
    for (int i = 0; i < l; ++i) {
        int u; fscanf(input_file, "%d", &u);
        list[i] = u;
    }

    init(n, list);
}