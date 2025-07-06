#ifndef FW_H
#define FW_H

#include "dssolver.h"

class FW: public DSSolver {
private:
    std::vector<double> r;
    std::vector<double> a;
public:

    ~FW() override;
    FW(const Graph *G_, const VertexSet *R_, const VertexSet *A_, const Setting &setting_);


    void frankWolfe();
    void findDS(int t) override;
    void solve() override;

} ;

#endif