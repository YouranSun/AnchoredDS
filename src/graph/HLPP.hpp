#ifndef HLPP_HPP
#define HLPP_HPP

#include <vector>
#include <queue>
#include <cstdio>

struct HLPP_noGlobal {
    /*
     * set ANS_TYPE to int or long long
     * nodes numbered by 1, 2, ..., n
     */
    typedef double ANS_TYPE; // or long long
    typedef unsigned Vertex;
    typedef long long Edge;

    struct StructEdge {
        Vertex to;
        ANS_TYPE c;
        long long rev;
        StructEdge(Vertex to=0, ANS_TYPE c=0, long long rev=0): to(to), c(c), rev(rev) {}
    };
    
    Vertex n, s, t;
    long long m;
    Vertex maxh, maxgaph, workcnt;
    std::vector<std::vector<StructEdge>> vec; // graph
    std::vector<ANS_TYPE> ov; // overflow of nodes
    std::vector<Vertex> h;  // height of nodes
    std::vector<long long> cur; // current arc
    std::vector<long long> ovList, ovNxt;
    std::vector<long long> gap, gapPrv, gapNxt;
    
    // n: nodes, m: edges, s: source node, t: sink node
    HLPP_noGlobal(Vertex n, long long m, Vertex s, Vertex t): n(n), m(m), s(s), t(t), maxh(0), maxgaph(0), workcnt(0),
                                      vec(n+1), ov(n+1), h(n+1), cur(n+1),
                                      ovList((n+1), -1), ovNxt(n+1, -1),
                                      gap((n+1), -1), gapPrv(n+1, -1), gapNxt(n+1, -1) {}

    ~HLPP_noGlobal() {
        std::vector<std::vector<StructEdge>>().swap(vec);
        std::vector<ANS_TYPE>().swap(ov);
        std::vector<Vertex>().swap(h);
        std::vector<long long>().swap(cur);
        std::vector<long long>().swap(ovList);
        std::vector<long long>().swap(ovNxt);
        std::vector<long long>().swap(gap);
        std::vector<long long>().swap(gapPrv);
        std::vector<long long>().swap(gapNxt);
    }

    void resize(Vertex u, unsigned sz) {
        vec[u].resize(sz);
        vec[u].clear();
    }
    void addEdge(Vertex u, Vertex v, ANS_TYPE c) {
        vec[u].push_back(StructEdge(v, c, vec[v].size()));
        vec[v].push_back(StructEdge(u, 0, (long long)(vec[u].size())-1));
    }
    void reSetEdge(Vertex u, Vertex v, Edge iu, Edge iv, ANS_TYPE c) {
        vec[u][iu].c = c;
        vec[v][iv].c = 0;
    }

    void addEdgePreFlow(Vertex u, Vertex v, ANS_TYPE c, ANS_TYPE f) {
        vec[u].push_back(StructEdge(v, c-f, vec[v].size()));
        vec[v].push_back(StructEdge(u, f, (long long)(vec[u].size())-1));
    }

    ANS_TYPE maxFlow() {
        globalRelabel();
        for(auto &e: vec[s]) if(e.c) { pushFlow(s, e, e.c);  maxh = std::max(maxh, h[e.to]); }
// printf("here\n");fflush(stdout);
        for(; ~maxh; --maxh) {
// printf("here %u\n", maxh);fflush(stdout);
            while(~ovList[maxh]) {
                long long x = ovList[maxh];
                ovList[maxh] = ovNxt[x];
                discharge(x);
                // if(workcnt > (n<<2))  globalRelabel();
            }
        }
        return ov[t];
    }

    void getSTCut(std::vector<unsigned> & vs) {
        // std::queue<Vertex> que;
        // std::vector<bool> vis(n);
        vs.clear();

        // for(auto &e: vec[s]) if(e.c > 1e-5) {
        //     que.push(e.to);
        //     vs.push_back(e.to);
        //     vis[e.to] = true;
        // }
        // while(!que.empty()) {
        //     Vertex u = que.front();
        //     que.pop();

        //     for(auto &e: vec[s]) if(e.c>1e-5 && !vis[e.to]) {
        //         que.push(e.to);
        //         vs.push_back(e.to);
        //         vis[e.to] = true;
        //     }
        // }
// printf("height:");
// for(Vertex u = 0; u < n; u++) {
//     printf("%u:%u ", u, h[u]);
// }printf("\n");
        // for(auto & e : vec[t]) {
        //     if(vec[e.to][e.rev].c < 1e-5) vs.push_back(e.to);
        // }
        globalRelabel();
        for(Vertex u = 1; u < n-1; u++) {
            if(h[u] == n) vs.push_back(u);
        }
    }

    void getTCut(std::vector<unsigned> & vs) {
        vs.clear();
        for(Vertex u = 1; u < n-1; u++) {
            if(h[u] != n) vs.push_back(u);
        }
    }

    void print() {
        for(Vertex u = 0; u < n; u++) {
            printf("%u ", u);
            for(auto & e : vec[u]) {
                printf("%u ", e.to);
            }
            printf("\n");
        }
    }
    void print2() {
        for(Vertex u = 0; u < n; u++) {
            printf("%u ", u);
            for(auto & e : vec[u]) {
                printf("%u:%.2f ", e.to, e.c);
            }
            printf("\n");
        }
    }
private:
    void discharge(Vertex x) {
        Vertex nh = n;
        long long sz = vec[x].size();
        for(long long i = cur[x]; i < sz; ++i) {
            auto &e = vec[x][i];
            if(e.c > 0) {
                if(h[x] == h[e.to]+1) {
                    pushFlow(x, e, std::min(ov[x], e.c));
                    if(ov[x] == 0) { cur[x] = i;  return ; }
                } else nh = std::min(nh, h[e.to]+1);
            }
        }
        for(long long i = 0; i < cur[x]; ++i) {
            auto &e = vec[x][i];
            if(e.c > 0) { nh = std::min(nh, h[e.to]+1); }
        }
        cur[x] = 0;
        ++workcnt;
        if(~gapNxt[gap[h[x]]])  setHeight(x, nh);
        else {
            long long oldh = h[x];
            for(long long i = oldh; i <= maxgaph; ++i) {
                for(long long j = gap[i]; ~j; j = gapNxt[j])  h[j] = n;
                gap[i] = -1;
            }
            maxgaph = oldh-1;
        }
    }
    
    void globalRelabel() {
        workcnt = maxh = maxgaph = 0;
        std::fill(h.begin(), h.end(), n);  h[t] = 0;
        std::fill(gapPrv.begin(), gapPrv.end(), -1);
        std::fill(gapNxt.begin(), gapNxt.end(), -1);
        std::fill(gap.begin(), gap.end(), -1);
        std::fill(ov.begin(), ov.end(), 0);
        std::fill(ovList.begin(), ovList.end(), -1);
        std::fill(ovNxt.begin(), ovNxt.end(), -1);
        std::fill(cur.begin(), cur.end(), 0);
        std::queue<long long> que;  que.push(t);
        long long x;
        while(!que.empty()) {
            x = que.front();  que.pop();
            for(auto &e: vec[x]) {
                if(h[e.to] == n && e.to != s && vec[e.to][e.rev].c > 0) {
                    setHeight(e.to, h[x]+1);
                    que.push(e.to);
                }
            }
        }
    }

    void setHeight(long long x, long long newh) {
        if(~gapPrv[x]) {
            if(gapPrv[x] == x) {
                gapPrv[gapNxt[x]] = gapNxt[x];
                gap[h[x]] = gapNxt[x];
            } else {
                gapNxt[gapPrv[x]] = gapNxt[x];
                if(~gapNxt[x])  gapPrv[gapNxt[x]] = gapPrv[x];
            }
        }
        if((h[x] = newh) >= n)  return ; // ignore the case of h >= n
        maxgaph = std::max(maxgaph, h[x]);
        if(ov[x] > 0) { maxh = std::max(maxh, h[x]);  ovNxt[x] = ovList[h[x]];  ovList[h[x]] = x; }
        if(~(gapNxt[x] = gap[h[x]]))  gapPrv[gapNxt[x]] = x;
        gap[h[x]] = gapPrv[x] = x;
    }
    
    void pushFlow(long long from, StructEdge &e, ANS_TYPE flow) {
        if(!ov[e.to] && e.to != t) {
            ovNxt[e.to] = ovList[h[e.to]];
            ovList[h[e.to]] = e.to;
        }
        e.c -= flow;
        vec[e.to][e.rev].c += flow;
        ov[from] -= flow;
        ov[e.to] += flow;
    }
};

struct HLPP {
    /*
     * set ANS_TYPE to int or long long
     * nodes numbered by 1, 2, ..., n
     */
    typedef double ANS_TYPE; // or long long
    typedef unsigned Vertex;
    typedef long long Edge;

    struct StructEdge {
        Vertex to;
        ANS_TYPE c;
        long long rev;
        StructEdge(Vertex to=0, ANS_TYPE c=0, long long rev=0): to(to), c(c), rev(rev) {}
    };
    
    Vertex n, s, t;
    long long m;
    Vertex maxh, maxgaph, workcnt;
    std::vector<std::vector<StructEdge>> vec; // graph
    std::vector<ANS_TYPE> ov; // overflow of nodes
    std::vector<Vertex> h;  // height of nodes
    std::vector<long long> cur; // current arc
    std::vector<long long> ovList, ovNxt;
    std::vector<long long> gap, gapPrv, gapNxt;
    
    // n: nodes, m: edges, s: source node, t: sink node
    HLPP(Vertex n, long long m, Vertex s, Vertex t): n(n), m(m), s(s), t(t), maxh(0), maxgaph(0), workcnt(0),
                                      vec(n+1), ov(n+1), h(n+1), cur(n+1),
                                      ovList((n+1), -1), ovNxt(n+1, -1),
                                      gap((n+1), -1), gapPrv(n+1, -1), gapNxt(n+1, -1) {}

    ~HLPP() {
        std::vector<std::vector<StructEdge>>().swap(vec);
        std::vector<ANS_TYPE>().swap(ov);
        std::vector<Vertex>().swap(h);
        std::vector<long long>().swap(cur);
        std::vector<long long>().swap(ovList);
        std::vector<long long>().swap(ovNxt);
        std::vector<long long>().swap(gap);
        std::vector<long long>().swap(gapPrv);
        std::vector<long long>().swap(gapNxt);
    }

    void resize(Vertex u, unsigned sz) {
        vec[u].resize(sz);
        vec[u].clear();
    }
    void addEdge(Vertex u, Vertex v, ANS_TYPE c) {
        vec[u].push_back(StructEdge(v, c, vec[v].size()));
        vec[v].push_back(StructEdge(u, 0, (long long)(vec[u].size())-1));
    }
    void reSetEdge(Vertex u, Vertex v, Edge iu, Edge iv, ANS_TYPE c) {
        vec[u][iu].c = c;
        vec[v][iv].c = 0;
    }

    void addEdgePreFlow(Vertex u, Vertex v, ANS_TYPE c, ANS_TYPE f) {
        vec[u].push_back(StructEdge(v, c-f, vec[v].size()));
        vec[v].push_back(StructEdge(u, f, (long long)(vec[u].size())-1));
    }

    ANS_TYPE maxFlow() {
        globalRelabel();
        for(auto &e: vec[s]) if(e.c) { pushFlow(s, e, e.c);  maxh = std::max(maxh, h[e.to]); }
// printf("here\n");fflush(stdout);
        for(; ~maxh; --maxh) {
// printf("here %u\n", maxh);fflush(stdout);
            while(~ovList[maxh]) {
                long long x = ovList[maxh];
                ovList[maxh] = ovNxt[x];
                discharge(x);
                if(workcnt > (n<<2))  globalRelabel();
            }
        }
        return ov[t];
    }

    void getSTCut(std::vector<unsigned> & vs) {
        // std::queue<Vertex> que;
        // std::vector<bool> vis(n);
        vs.clear();

        // for(auto &e: vec[s]) if(e.c > 1e-5) {
        //     que.push(e.to);
        //     vs.push_back(e.to);
        //     vis[e.to] = true;
        // }
        // while(!que.empty()) {
        //     Vertex u = que.front();
        //     que.pop();

        //     for(auto &e: vec[s]) if(e.c>1e-5 && !vis[e.to]) {
        //         que.push(e.to);
        //         vs.push_back(e.to);
        //         vis[e.to] = true;
        //     }
        // }
// printf("height:");
// for(Vertex u = 0; u < n; u++) {
//     printf("%u:%u ", u, h[u]);
// }printf("\n");
        // for(auto & e : vec[t]) {
        //     if(vec[e.to][e.rev].c < 1e-5) vs.push_back(e.to);
        // }
        globalRelabel();
        for(Vertex u = 1; u < n-1; u++) {
            if(h[u] == n) vs.push_back(u);
        }
    }

    void getTCut(std::vector<unsigned> & vs) {
        vs.clear();
        for(Vertex u = 1; u < n-1; u++) {
            if(h[u] != n) vs.push_back(u);
        }
    }

    void print() {
        for(Vertex u = 0; u < n; u++) {
            printf("%u ", u);
            for(auto & e : vec[u]) {
                printf("%u ", e.to);
            }
            printf("\n");
        }
    }
    void print2() {
        for(Vertex u = 0; u < n; u++) {
            printf("%u ", u);
            for(auto & e : vec[u]) {
                printf("%u:%.2f ", e.to, e.c);
            }
            printf("\n");
        }
    }
private:
    void discharge(Vertex x) {
        Vertex nh = n;
        long long sz = vec[x].size();
        for(long long i = cur[x]; i < sz; ++i) {
            auto &e = vec[x][i];
            if(e.c > 0) {
                if(h[x] == h[e.to]+1) {
                    pushFlow(x, e, std::min(ov[x], e.c));
                    if(ov[x] == 0) { cur[x] = i;  return ; }
                } else nh = std::min(nh, h[e.to]+1);
            }
        }
        for(long long i = 0; i < cur[x]; ++i) {
            auto &e = vec[x][i];
            if(e.c > 0) { nh = std::min(nh, h[e.to]+1); }
        }
        cur[x] = 0;
        ++workcnt;
        if(~gapNxt[gap[h[x]]])  setHeight(x, nh);
        else {
            long long oldh = h[x];
            for(long long i = oldh; i <= maxgaph; ++i) {
                for(long long j = gap[i]; ~j; j = gapNxt[j])  h[j] = n;
                gap[i] = -1;
            }
            maxgaph = oldh-1;
        }
    }
    
    void globalRelabel() {
        workcnt = maxh = maxgaph = 0;
        std::fill(h.begin(), h.end(), n);  h[t] = 0;
        std::fill(gapPrv.begin(), gapPrv.end(), -1);
        std::fill(gapNxt.begin(), gapNxt.end(), -1);
        std::fill(gap.begin(), gap.end(), -1);
        std::fill(ov.begin(), ov.end(), 0);
        std::fill(ovList.begin(), ovList.end(), -1);
        std::fill(ovNxt.begin(), ovNxt.end(), -1);
        std::fill(cur.begin(), cur.end(), 0);
        std::queue<long long> que;  que.push(t);
        long long x;
        while(!que.empty()) {
            x = que.front();  que.pop();
            for(auto &e: vec[x]) {
                if(h[e.to] == n && e.to != s && vec[e.to][e.rev].c > 0) {
                    setHeight(e.to, h[x]+1);
                    que.push(e.to);
                }
            }
        }
    }

    void setHeight(long long x, long long newh) {
        if(~gapPrv[x]) {
            if(gapPrv[x] == x) {
                gapPrv[gapNxt[x]] = gapNxt[x];
                gap[h[x]] = gapNxt[x];
            } else {
                gapNxt[gapPrv[x]] = gapNxt[x];
                if(~gapNxt[x])  gapPrv[gapNxt[x]] = gapPrv[x];
            }
        }
        if((h[x] = newh) >= n)  return ; // ignore the case of h >= n
        maxgaph = std::max(maxgaph, h[x]);
        if(ov[x] > 0) { maxh = std::max(maxh, h[x]);  ovNxt[x] = ovList[h[x]];  ovList[h[x]] = x; }
        if(~(gapNxt[x] = gap[h[x]]))  gapPrv[gapNxt[x]] = x;
        gap[h[x]] = gapPrv[x] = x;
    }
    
    void pushFlow(long long from, StructEdge &e, ANS_TYPE flow) {
        if(!ov[e.to] && e.to != t) {
            ovNxt[e.to] = ovList[h[e.to]];
            ovList[h[e.to]] = e.to;
        }
        e.c -= flow;
        vec[e.to][e.rev].c += flow;
        ov[from] -= flow;
        ov[e.to] += flow;
    }
};

struct HLPP_MemoryOptimized {
    /*
     * set ANS_TYPE to long long or long long
     * nodes numbered by 1, 2, ..., n
     */
    typedef double ANS_TYPE; // or long long
    typedef int Node_TYPE;
    struct StructEdge {
        Node_TYPE to;
        ANS_TYPE c;
        Node_TYPE rev;
        StructEdge(Node_TYPE to=0, ANS_TYPE c=0, Node_TYPE rev=0): to(to), c(c), rev(rev) {}
    };
    
    Node_TYPE n, s, t;
    ANS_TYPE m;
    Node_TYPE maxh, maxgaph, workcnt;
    // std::vector<std::vector<StructEdge>> vec; // graph
    std::vector<StructEdge> pEdge;//memory pool
    std::vector<Node_TYPE> pIdx;

    std::vector<ANS_TYPE> ov; // overflow of nodes
    std::vector<Node_TYPE> h;  // height of nodes
    std::vector<Node_TYPE> cur; // current arc
    std::vector<Node_TYPE> ovList, ovNxt;
    std::vector<Node_TYPE> gap, gapPrv, gapNxt;
    std::queue<Node_TYPE> que; 

    struct Queue
    {
        std::vector<long long> a;
        long long l = 0, r = 0;
        Queue(long long N = 1e6) {
            a.resize(N);
            l = r = 0;
        }
        ~Queue() {
            std::vector<long long>().swap(a);
        }
    
        void push(long long x)
        {
            a[r++] = x;
            if(r == a.size()) {
                r = 0;
            }
            if(r == l) {//full
                std::vector<long long> b(a.size()*2);
                for(long long i = l, j = 0; i != r; ++j, i = (i+1) % a.size()) {
                    b[j] = a[i];
                }
                l = 0;
                r = a.size();
                a.swap(b);
                std::vector<long long>().swap(b);
            }
        }
        long long front() {
            return a[l];
        }
        void pop()
        {
            l++;
            if(l == a.size()) l = 0;
        }
        bool empty()
        {
            return r == l;
        }
        long long size() {
            return (r - l + a.size()) % a.size();
        }
    }; 
    // Queue que;
    
    // n: nodes, m: edges, s: source node, t: sink node
    HLPP_MemoryOptimized(Node_TYPE n, ANS_TYPE m, Node_TYPE s, Node_TYPE t): n(n), m(m), s(s), t(t), maxh(0), maxgaph(0), workcnt(0),
                                    ov(n+1), h(n+1), cur(n+1),
                                      ovList((n+1), -1), ovNxt(n+1, -1),
                                      gap((n+1), -1), gapPrv(n+1, -1), gapNxt(n+1, -1) {}
    ~HLPP_MemoryOptimized() {
        using T = Node_TYPE;
        std::vector<StructEdge>().swap(pEdge);
        std::vector<T>().swap(pIdx);
        std::vector<ANS_TYPE>().swap(ov);
        std::vector<T>().swap(h);
        std::vector<T>().swap(cur);
        std::vector<T>().swap(ovList);
        std::vector<T>().swap(ovNxt);
        std::vector<T>().swap(gap);
        std::vector<T>().swap(gapPrv);
        std::vector<T>().swap(gapNxt);
    }
    // void resize(ui sz) {
    //     pEdge.resize(sz);
    // }
    //build graph out of the struct
    void addEdge(Node_TYPE u, Node_TYPE v, ANS_TYPE c) {
        pEdge[pIdx[u]].to = v;
        pEdge[pIdx[u]].c = c;
        pEdge[pIdx[u]].rev = pIdx[v];

        pEdge[pIdx[v]].to = u;
        pEdge[pIdx[v]].c = 0;
        pEdge[pIdx[v]].rev = pIdx[u];

        pIdx[u]++;
        pIdx[v]++;
    }
    void addEdgePreFlow(Node_TYPE u, Node_TYPE v, ANS_TYPE c, ANS_TYPE f) {
        pEdge[pIdx[u]].to = v;
        pEdge[pIdx[u]].c = c-f;
        pEdge[pIdx[u]].rev = pIdx[v];

        pEdge[pIdx[v]].to = u;
        pEdge[pIdx[v]].c = f;
        pEdge[pIdx[v]].rev = pIdx[u];

        pIdx[u]++;
        pIdx[v]++;
    }
    // void addStructEdge(long long u, long long v, ANS_TYPE c) {
    //     vec[u].push_back(StructEdge(v, c, vec[v].size()));
    //     vec[v].push_back(StructEdge(u, 0, long long(vec[u].size())-1));
    // }
    // void addStructEdgePreFlow(long long u, long long v, ANS_TYPE c, ANS_TYPE f) {
    //     vec[u].push_back(StructEdge(v, c-f, vec[v].size()));
    //     vec[v].push_back(StructEdge(u, f, long long(vec[u].size())-1));
    // }

    ANS_TYPE maxFlow() {
// printf("here0\n");fflush(stdout);
        globalRelabel();
// printf("here1\n");fflush(stdout);
        // for(auto &e: vec[s])
        for(Node_TYPE i = pIdx[s]; i < pIdx[s+1]; i++) {
            auto &e = pEdge[i];
            if(e.c) { 
                pushFlow(s, e, e.c);
                maxh = std::max(maxh, h[e.to]);
            }
        }
// printf("here2 %ld\n", maxh);fflush(stdout);
        for(; maxh >= 0; --maxh) {
// printf("here20 %ld %lld\n", maxh, workcnt);fflush(stdout);
            while(~ovList[maxh]) {
                long long x = ovList[maxh];
// if(ovNxt.size() <= x) {
//     printf("err1 %lld\n", x);fflush(stdout);
// }
                ovList[maxh] = ovNxt[x];
                discharge(x);

                if(workcnt > (n<<2)) {
// printf("here20a %ld\n", maxh);fflush(stdout);
                    globalRelabel();
// printf("here20b %ld\n", maxh);fflush(stdout);
                }
            }
        }
// printf("here3\n");fflush(stdout);
        return ov[t];
    }
private:
    void discharge(Node_TYPE x) {
        // long long nh = n, sz = vec[x].size();
        Node_TYPE nh = n, sz = pIdx[x+1] - pIdx[x];
        for(Node_TYPE i = cur[x]; i < sz; ++i) {
            // auto &e = vec[x][i];
            auto &e = pEdge[pIdx[x] + i];
            if(e.c > 0) {
                if(h[x] == h[e.to]+1) {
                    pushFlow(x, e, std::min(ov[x], e.c));
                    if(ov[x] == 0) { cur[x] = i;  return ; }
                } else nh = std::min(nh, h[e.to]+1);
            }
        }
        for(Node_TYPE i = 0; i < cur[x]; ++i) {
            // auto &e = vec[x][i];
            auto &e = pEdge[pIdx[x] + i];
            if(e.c > 0) { nh = std::min(nh, h[e.to]+1); }
        }
        cur[x] = 0;
        ++workcnt;
        if(~gapNxt[gap[h[x]]])  setHeight(x, nh);
        else {
            Node_TYPE oldh = h[x];
            for(Node_TYPE i = oldh; i <= maxgaph; ++i) {
                for(Node_TYPE j = gap[i]; ~j; j = gapNxt[j])  h[j] = n;
                gap[i] = -1;
            }
            maxgaph = oldh-1;
        }
    }
    
    void globalRelabel() {
        workcnt = maxh = maxgaph = 0;
        std::fill(h.begin(), h.end(), n);  h[t] = 0;
        std::fill(gapPrv.begin(), gapPrv.end(), -1);
        std::fill(gapNxt.begin(), gapNxt.end(), -1);
        std::fill(gap.begin(), gap.end(), -1);
        std::fill(ovList.begin(), ovList.end(), -1);
        std::fill(ovNxt.begin(), ovNxt.end(), -1);
        std::fill(cur.begin(), cur.end(), 0);
        
        while(!que.empty()) que.pop();
        que.push(t);

        Node_TYPE x;
// printf("here01\n");fflush(stdout);
// ui ttt = 0;
        while(!que.empty()) {
            x = que.front();  que.pop();
            // for(auto &e: vec[x]) {
// if(ttt % 2 == 0) {

// printf("here01 %lld %lu %lld %lld\n", x, que.size(), que.l, que.r);fflush(stdout);
// ttt++;
// }
            for(Node_TYPE i = pIdx[x]; i < pIdx[x+1]; i++) {
// if(i >= 25030386) {
// printf("herea %lld %lld\n", i, pIdx[x+1]);fflush(stdout);
// }
// assert(i < pEdge.size());
                auto &e = pEdge[i];
                // if(h[e.to] == n && e.to != s && vec[e.to][e.rev].c > 0) {
                auto &re = pEdge[e.rev];
// if(i >= 25030386) {
// printf("hereb %lld %lld\n", i, pIdx[x+1]);fflush(stdout);
// }
                if(h[e.to] == n && e.to != s && re.c > 0) {
                    setHeight(e.to, h[x]+1);
                    que.push(e.to);
                }
// if(i >= 25030386) {
// printf("herec %lld %lld\n", i, pIdx[x+1]);fflush(stdout);
// }
            }
        }
// printf("here02\n");fflush(stdout);
    }

    void setHeight(Node_TYPE x, Node_TYPE newh) {
        if(~gapPrv[x]) {
            if(gapPrv[x] == x) {
                gapPrv[gapNxt[x]] = gapNxt[x];
                gap[h[x]] = gapNxt[x];
            } else {
                gapNxt[gapPrv[x]] = gapNxt[x];
                if(~gapNxt[x])  gapPrv[gapNxt[x]] = gapPrv[x];
            }
        }
        if((h[x] = newh) >= n)  return ; // ignore the case of h >= n
        maxgaph = std::max(maxgaph, h[x]);
        if(ov[x] > 0) { maxh = std::max(maxh, h[x]);  ovNxt[x] = ovList[h[x]];  ovList[h[x]] = x; }
        if(~(gapNxt[x] = gap[h[x]]))  gapPrv[gapNxt[x]] = x;
        gap[h[x]] = gapPrv[x] = x;
    }
    
    void pushFlow(Node_TYPE from, StructEdge &e, ANS_TYPE flow) {
        if(!ov[e.to] && e.to != t) {
            ovNxt[e.to] = ovList[h[e.to]];
            ovList[h[e.to]] = e.to;
        }
        e.c -= flow;
        // vec[e.to][e.rev].c += flow;
        pEdge[e.rev].c += flow;
        ov[from] -= flow;
        ov[e.to] += flow;
    }
};


#endif