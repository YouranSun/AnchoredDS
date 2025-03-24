#ifndef FDP_H
#define FDP_H

#include "dssolver.h"

class FDP: public DSSolver {
private:
    std::vector<unsigned int> r;
    std::vector<unsigned int> a;
public:

    ~FDP() override;
    FDP(const Graph *G_, const VertexSet *R_, const VertexSet *A_, const Setting &setting_);


    void frankWolfe();
    void findDS(int t) override;
    void solve() override;

} ;

#endif