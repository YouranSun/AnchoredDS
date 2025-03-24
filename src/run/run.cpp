#include "../tools/getArgs.hpp"
#include "../tools/utils.hpp"
#include "../graph/graph.h"
#include "../solvers/fista.h"
#include "../solvers/fdp.h"
#include "../nrcore/nrcore.h"
#include "setting.h"

#include <cassert>
#include <cstdio>


int main(int argc, char **argv) {
    argsController ac(argc, argv);

    Setting setting(ac);

    Graph *G = read<Graph>(setting.path_G);
    VertexSet *R = read<VertexSet>(setting.path_R);
    VertexSet *A = read<VertexSet>(setting.path_A);

    // if (use_core) {
    //     NRCore *C = new NRCore(G, R, A);
    //     C -> nrCore(G, R, A);

    //     eprintf("CORE GRAPH\tnew size = %d\n", G -> n);

    //     write<Graph>(G, ac, "-gp");
    //     write<VertexSet>(R, ac, "-rp");
    //     write<VertexSet>(A, ac, "-ap");
    //     delete C;
    // }

    DSSolver *solver = nullptr;

    if (setting.is_fista) {
        solver = new Fista(G, R, A, setting);
    }
    else{
        solver = new FDP(G, R, A, setting);
    }
    
    solver -> solve();

    printf("SOLVER\tdensity = %.10lf\n", solver -> best_rho);


    solver -> printResults();


    delete solver;
    delete A;
    delete R;
    delete G;
    return 0;
}