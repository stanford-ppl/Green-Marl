#include "common_main.h"
#include "randomnodesampling.h"

class my_main: public main_t
{
public:

    virtual ~my_main() {}

    virtual bool prepare() {
        return true;
    }

    virtual bool run() {
        int x = rand() % G.num_nodes();
        gm_node_set set(G.num_nodes());
        gm_node_set set2(G.num_nodes());
        gm_node_set set3(G.num_nodes());
        RandomNodeSampling(G, x, set);
        RandomDegreeNodeSampling(G, x, set);
        RandomWalkSamplingWithRandomJump(G, x, 0.2, set);
        return true;
    }

    virtual bool post_process() {
        return true;
    }
};

int main(int argc, char** argv) {
    my_main M;
    M.main(argc, argv);
}
