#include "common_main.h"
#include "sssp_path.h"

class my_main: public main_t
{
public:

    virtual ~my_main() {}

    virtual bool prepare() {
        return true;
    }

    virtual bool run() {
        node_t root = 0;
        node_t begin = 0;
        node_t end = 0;
        gm_node_seq Q;
        sssp_path(G, NULL, NULL, root, NULL);
        get_path(G, begin ,end, NULL, Q);
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
