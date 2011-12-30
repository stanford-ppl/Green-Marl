
#include "common_main.h"
#include "b2.h"

void P(gm_graph& G, int32_t* G_A, int32_t* G_B, node_t& k);

class b2_main: public main_t
{
public:
    int32_t* A;
    int32_t* B;
    node_t c;

    virtual bool create_data() {
        A = new int32_t[G.num_nodes()];
        B = new int32_t[G.num_nodes()];
        c = 0;
        return true;
    }

    virtual bool run() {
        P(G, A, B, c);
        return true;
    }
};

int main(int argc, char** argv)
{
    b2_main M;
    M.main(argc, argv);
}
