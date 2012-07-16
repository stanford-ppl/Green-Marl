#include "common_main.h"
#include "sssp_path.h"

class my_main: public main_t
{
private:
    int* dist;
    int* len;
    node_t* prev1;
    node_t* prev2;
public:
    virtual ~my_main() {
        delete[] dist;
        delete[] len;
        delete[] prev1;
        delete[] prev2;
    }

    virtual bool prepare() {
        dist = new int[G.num_nodes()];
        len = new int[G.num_edges()];
        prev1 = new node_t[G.num_nodes()];
        prev2 = new node_t[G.num_nodes()];
        return true;
    }

    virtual bool run() {
        node_t root = 0;
        node_t begin = 0;
        node_t end = 0;
        gm_node_seq Q;
        sssp_path(G, dist, len, root, prev1);
        get_path(G, begin, end, prev2, Q);
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
