#include "common_main.h"
#include "random_node_sampling.h"

class my_main: public main_t
{
public:

    virtual bool prepare() {
        return true;
    }

    virtual bool run() {
        int start = rand() % G.num_nodes();
        gm_node_set set(G.num_nodes());
        random_node_sampling(G, start, set);
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
