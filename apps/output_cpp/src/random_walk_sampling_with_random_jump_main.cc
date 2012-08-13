#include "common_main.h"
#include "random_walk_sampling_with_random_jump.h"

class my_main: public main_t
{
public:

    virtual bool prepare() {
        return true;
    }

    virtual bool run() {
        int start = rand() % G.num_nodes();
        gm_node_set set(G.num_nodes());
        random_walk_sampling_with_random_jump(G, start, 0.15, set);
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
