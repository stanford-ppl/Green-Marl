
#include "common_main.h"
#include "b2.h"

void P(gm_graph& G, int32_t* G_A, int32_t* G_B, node_t& k);

class b2_main: public main_t
{
public:
    int32_t* A;
    int32_t* B;
    node_t c;

    virtual ~b2_main() {
        delete[] A;
        delete[] B;
    }

    virtual bool prepare() {
        A = new int32_t[G.num_nodes()];
        B = new int32_t[G.num_nodes()];
        c = 0;
        return true;
    }

    virtual bool run() {
        P(G, A, B, c);
        return true;
    }

    virtual bool post_process() {
        return true;
    }
};

int main(int argc, char** argv)
{
#if 1
    b2_main M;
    M.main(argc, argv);
#else
    gm_graph G;
    for(int i=0;i<5;i++)
        G.add_node();

    G.add_edge(0,1);
    G.add_edge(1,2);
    G.add_edge(2,3);
    G.add_edge(0,4);

    int32_t* A;
    int32_t* B;
    node_t c;

    A = new int32_t[G.num_nodes()];
    B = new int32_t[G.num_nodes()];
    c = 0;

    P(G, A, B, c);
#endif
}
