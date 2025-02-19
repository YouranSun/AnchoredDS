#include "../tools/getArgs.hpp"
#include "../tools/utils.hpp"
#include "../graph/graph.h"
#include "../fista/fista.h"

#include <cassert>
#include <cstdio>

template<typename Arg> Arg *read(const argsController &ac, const std::string &arg_name) {
    assert(ac.exist(arg_name));
    Arg *arg = new Arg();
    arg -> readFromText(ac[arg_name].c_str());
    eprintf("finished read from text\n");
    return arg;
}

int main(int argc, char **argv) {
    argsController ac(argc, argv);

    Graph *G = read<Graph>(ac, "-g");
    VertexSet *R = read<VertexSet>(ac, "-r");
    VertexSet *A = read<VertexSet>(ac, "-a");

    Fista *fista = new Fista(G, R, A);
    fista -> solve();
    
    for (auto u: (fista -> densestSubgraph())) {
        printf("%d ", u);
    }
    puts("");
    return 0;
}