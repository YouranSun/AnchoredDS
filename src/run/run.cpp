#include "../tools/getArgs.hpp"
#include "../tools/utils.hpp"
#include "../graph/graph.h"
#include "../solvers/fista.h"
#include "../solvers/fdp.h"
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

    bool use_core = ac.exist("-c");

    assert(ac.exist("-d"));
    std::string method = ac["-d"];

    if (use_core) {
        NRCore *C = new NRCore(G, R, A);
        C -> nrCore();

        eprintf("CORE GRAPH\tnew size = %d\n", G -> n);

        delete C;
    }

    DSSolver *solver = nullptr;

    if (method == "FISTA") {
        solver = new Fista(G, R, A);
    }
    else if (method == "FDP"){
        solver = new FDP(G, R, A);
    }
    else {
        assert(false);
    }
    
    solver -> solve();

    printf("SOLVER\tdensity = %.10lf\n", solver -> best_rho);

    for (auto u: (solver -> densestSubgraph())) {
        printf("%d ", u);
    }
    puts("");

    delete solver;
    delete A;
    delete R;
    delete G;
    return 0;
}