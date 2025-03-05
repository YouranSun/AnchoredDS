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

std::mt19937 rnd(2226701);

std::vector<unsigned int> generateA(Graph *G, unsigned int s, unsigned int cnt, std::vector<bool> &vis) {
    std::vector<unsigned int> pools;
    for(auto [u, e1]: (G -> g)[s]) {
        for (auto [v, e2]: (G -> g)[u]) {
            if(vis[v]) continue;
            pools.push_back(v);
            vis[v] = true;
        }
    }

    for(auto v :pools) vis[v] = false;

    std::vector<unsigned int> res(cnt);
    std::uniform_int_distribution<unsigned int> sp(0, pools.size()-1);

    for(unsigned int i = 0; i < cnt; i++) {
        do {
            unsigned int v = pools[sp(rnd)];
            res[i] = v;
        } while(vis[res[i]]);
        vis[res[i]] = true;
    }

    for(unsigned int u : res) vis[u] = false;

    return res;
}

std::vector<unsigned int> generateR(
            Graph *G, std::vector<unsigned int> &vA, 
            unsigned int DEF_ANCHOR_REPEATS, unsigned int STEPS,
            std::vector<bool> & vis
    ) {
    auto getRNodeDegreeCap = [&](unsigned int c_max, unsigned int N) {
        double scale = std::min(
            (double)UINT32_MAX, 
            std::max(2.0, std::log(N / c_max)/std::log(2))
        );
        return scale * c_max;
    };
    
    unsigned int max_deg_in_A = 0;
    for(auto u : vA) max_deg_in_A = std::max(max_deg_in_A, (unsigned int)(G -> g)[u].size());
    double r_degree_cap = getRNodeDegreeCap(
        max_deg_in_A, (G -> n));
    
    std::mt19937 gg(rnd());
    typedef std::uniform_int_distribution<unsigned int> distr_t;
    typedef typename distr_t::param_type param_t;
    distr_t D;
    
    std::vector<unsigned int> res;

    for(auto v : vA) {
        res.push_back(v);
        vis[v] = true;
        for(unsigned int i = 0; i < DEF_ANCHOR_REPEATS; i++) {
            unsigned int current = v;
            for(unsigned int step = 0; step < STEPS; step++) {
                unsigned int deg = (G -> g)[current].size();
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

void writeTo(std::vector<unsigned int> & V, std::string out_path) {
    FILE *out_file = fopen(out_path.c_str(), "w");
    std::sort(V.begin(), V.end());
    fprintf(out_file, "%d\n", (int)V.size());
    for(unsigned int u: V) fprintf(out_file, "%d ", u);
    fputs("", out_file);
    fclose(out_file);
}

int main(int argc, char * argv[]) {
    argsController ac(argc, argv);

    Graph *G = read<Graph>(ac, "-g");
    
    unsigned int generateCntR = 254;

    std::vector<bool> vis(G -> n);
    
    int s = rnd() % (G -> n);
    std::vector<unsigned int> vA = generateA(G, s, 8, vis);


    unsigned int DEF_ANCHOR_REPEATS = rnd() % 8 + 2;
    unsigned int STEPS = rnd() % 8 + 2;
    std::vector<unsigned int> vR = generateR(
        G, vA, DEF_ANCHOR_REPEATS, STEPS, vis);

    writeTo(vA, ac["-a"]);
    writeTo(vR, ac["-r"]);
    return 0;
}