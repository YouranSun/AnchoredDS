#include "../tools/getArgs.hpp"
#include "../tools/utils.hpp"
#include "../graph/graph.h"
#include "../solvers/fista.h"
#include "../solvers/fdp.h"
#include "../solvers/fw.h"
#include "../nrcore/nrcore.h"
#include "setting.h"

#include <cassert>
#include <cstdio>
#include <ctime>


int main(int argc, char **argv) {
    argsController ac(argc, argv);

    Setting setting(ac);

    Graph *G = new Graph(); std::map<size_t, size_t> ord = G -> readFromText(setting.path_G.c_str());
    VertexSet *R = read<VertexSet>(setting.path_R); R -> reorder(ord);
    VertexSet *A = read<VertexSet>(setting.path_A); A -> reorder(ord);

    // if (use_core) {
    //     NRCore *C = new NRCore(G, R, A);
    //     C -> nrCore(G, R, A);

    //     eprintf("CORE GRAPH\tnew size = %d\n", G -> n);

    //     write<Graph>(G, ac, "-gp");
    //     write<VertexSet>(R, ac, "-rp");
    //     write<VertexSet>(A, ac, "-ap");
    //     delete C;
    // }

    setting.start_time = clock();
    DSSolver *solver = nullptr;

    if (setting.is_fista) {
        solver = new Fista(G, R, A, setting);
    }
    else if (setting.is_fw) {
        solver = new FW(G, R, A, setting);
    }
    else {
        solver = new FDP(G, R, A, setting);
    }
    
    solver -> solve();

    // printf("SOLVER\tdensity = %.10lf\n", solver -> best_rho);


    solver -> printResults();


    delete solver;
    delete A;
    delete R;
    delete G;
    return 0;
}