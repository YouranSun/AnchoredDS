#include "../tools/getArgs.hpp"
#include "../tools/utils.hpp"
#include "../graph/graph.h"
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <algorithm>
#include <cmath>
#include <vector>

std::vector<size_t> readFrom(std::string in_path) {
    std::vector<size_t> V;
    FILE *in_file = fopen(in_path.c_str(), "r");
    if (in_file == nullptr) return V;
    size_t n; fscanf(in_file, "%zu", &n);
    for (size_t i = 0; i < n; ++i) {
        size_t u; fscanf(in_file, "%zu", &u);
        V.push_back(u);
    }
    fclose(in_file);
    return V;
}

void writeTo(std::vector<size_t> &V, std::string out_path) {
    FILE *out_file = fopen(out_path.c_str(), "w");
    std::sort(V.begin(), V.end());
    fprintf(out_file, "%zu\n", V.size());
    for(auto u: V) fprintf(out_file, "%zu ", u);
    fputs("", out_file);
    fclose(out_file);
}

std::mt19937 rnd(2226701);

std::vector<size_t> generateA(Graph *G, size_t s, int cnt, std::vector<int> &vis) {
    std::vector<size_t> pools;
    for(auto [u, e1]: (G -> g)[s]) {
        for (auto [v, e2]: (G -> g)[u]) {
            if(vis[v]) continue;
            pools.push_back(v);
            vis[v] = true;
        }
    }

    for(auto v :pools) vis[v] = false;

    std::vector<size_t> res(cnt);
    std::uniform_int_distribution<int> sp(0, pools.size()-1);

    for(int i = 0; i < cnt; i++) {
        do {
            int v = pools[sp(rnd)];
            res[i] = v;
        } while(vis[res[i]]);
        vis[res[i]] = true;
    }

    for(auto u : res) vis[u] = false;

    return res;
}

std::vector<size_t> generateR(
            Graph *G, std::vector<size_t> &vA, 
            int DEF_ANCHOR_REPEATS, int STEPS,
            std::vector<int> & vis
    ) {
    auto getRNodeDegreeCap = [&](int c_max, int N) {
        double scale = std::min(
            (double)UINT32_MAX, 
            std::max(2.0, std::log(N / c_max)/std::log(2))
        );
        return scale * c_max;
    };
    
    size_t max_deg_in_A = 0;
    for(auto u : vA) max_deg_in_A = std::max(max_deg_in_A, (G -> g)[u].size());
    double r_degree_cap = getRNodeDegreeCap(
        max_deg_in_A, (G -> n));
    
    
    std::vector<size_t> res;

    for(auto v : vA) {
        res.push_back(v);
        vis[v] = true;
        for(int i = 0; i < DEF_ANCHOR_REPEATS; i++) {
            int current = v;
            for(int step = 0; step < STEPS; step++) {
                int deg = (G -> g)[current].size();
                if(deg == 0) break;
                
                current = (G -> g)[current][rnd() % deg].first;
                if((G -> g)[current].size() <= r_degree_cap) {
                    if(!vis[current]) {
                        res.push_back(current);
                        vis[current] = true;
                    }
                }
            }
        }
    }

    for(auto u : res) vis[u] = false;

    return res;
}

std::vector<size_t> generateSt(Graph *G, std::string path) {
    const int lim = 200;

    std::vector<size_t> u_list = readFrom(path);
    if (u_list.size() > 0) return u_list;

    double th = 0.6;

    std::vector<size_t> ord(G -> n);
    for (size_t u = 0; u < (G -> n); ++u) {
        ord[u] = u;
    }
    // shuffle(ord.begin(), ord.end(), rnd);

    std::vector<bool> isNei(G -> n); 
    for(auto u: ord) {
        size_t du = (G -> g)[u].size();
        if(du < 10) continue;

        eprintf("GEN\tu=%d\n", u);

        for (auto [v, e]: (G -> g)[u]) {
            isNei[v] = true;
        }

        size_t cnt = 0;
        for (auto [v, e1]: (G -> g)[u]) {
            for (auto [w, e2]: (G -> g)[v]) {
                // eprintf("GEN\tu=%d v=%d w=%d\n", u, v, w);
                if(isNei[w]) cnt++;
            }
        }

        eprintf("GEN\tu = %zu %lf\n", u, cnt / (1.0 * du * (du - 1) / 2));

        if(cnt / (1.0 * du * (du - 1) / 2) >= th) {
            u_list.push_back(u);
        }

        for (auto [v, e]: (G -> g)[u]) {
            isNei[v] = false;
        }

        if (u_list.size() > lim) {
            break;
        }
        
        eprintf("GEN\tu_list size = %zu\n", u_list.size());
    }

    writeTo(u_list, path);
    return u_list;
}

int main(int argc, char * argv[]) {
    argsController ac(argc, argv);

    Graph *G = read<Graph>(ac, "-g");

    if (ac.exist("-seed")) {
        rnd = std::mt19937(atoi(ac["-seed"].c_str()));
    }

    std::vector<size_t> u_list = generateSt(G, ac["-st"]);

    int generateCntR = 254;

    std::vector<int> vis(G -> n, 0);
    
    int s = u_list[rnd() % (int)(u_list.size())];
    std::vector<size_t> vA = generateA(G, s, 8, vis);


    int DEF_ANCHOR_REPEATS = rnd() % 8 + 2;
    int STEPS = rnd() % 8 + 2;
    std::vector<size_t> vR = generateR(
        G, vA, DEF_ANCHOR_REPEATS, STEPS, vis);

    VertexSet A; A.init(G -> n, vA);
    VertexSet R; R.init(G -> n, vR);

    A.writeToText(ac["-a"].c_str());
    R.writeToText(ac["-r"].c_str());

    delete G;
    return 0;
}