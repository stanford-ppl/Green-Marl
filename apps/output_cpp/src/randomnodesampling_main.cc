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
        gm_node_set set(G.num_nodes());
        RandomNodeSampling(G, 0, set);
        RandomDegreeNodeSampling(G, 0, set);
        RandomWalkSamplingWithRandomJump(G, 0, 0.0, set);
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
