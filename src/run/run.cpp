#include "../tools/getArgs.hpp"
#include "../tools/utils.hpp"
#include "../graph/graph.h"
#include "../fista/fista.h"
#include "../nrcore/nrcore.h"

#include <cassert>
#include <cstdio>

template<typename Arg> Arg *read(const argsController &ac, const std::string &arg_name) {
    assert(ac.exist(arg_name));
    Arg *arg = new Arg();
    arg -> readFromText(ac[arg_name].c_str());
    eprintf("FINISHED\tread from text\n");
    return arg;
}

int main(int argc, char **argv) {
    argsController ac(argc, argv);

    Graph *G = read<Graph>(ac, "-g");
    VertexSet *R = read<VertexSet>(ac, "-r");
    VertexSet *A = read<VertexSet>(ac, "-a");

    NRCore *C = new NRCore(G, R, A);
    C -> nrCore();

    eprintf("CORE GRAPH\tnew size = %d\n", G -> n);

    Fista *fista = new Fista(G, R, A);
    fista -> solve();
    
    printf("FISTA\tdensity = %lf\n", fista -> best_rho);

    // for (auto u: (fista -> densestSubgraph())) {
    //     printf("%d ", u);
    // }
    // puts("");

    delete fista;
    delete C;
    delete A;
    delete R;
    delete G;
    return 0;
}