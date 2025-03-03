#ifndef FDP_H
#define FDP_H

#include "dssolver.h"

class FDP: public DSSolver {
private:
    std::vector<unsigned int> r;
public:

    ~FDP() override;
    FDP(const Graph *G_, const VertexSet *R_, const VertexSet *A_);


    void frankWolfe();
    void findDS() override;
    void solve() override;

} ;

#endif