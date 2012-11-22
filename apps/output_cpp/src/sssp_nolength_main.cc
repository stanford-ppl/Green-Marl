#include "common_main.h"
#include "sssp_nolength.h"

class my_main: public main_t
{
public:
    int32_t* dist;  // distance of each node
    node_t root;

    virtual ~my_main() {
        delete[] dist;
    }

    //--------------------------------------------------------
    // create 4 groups randomly
    //--------------------------------------------------------
    virtual bool prepare() {
        root = 0;
        dist = new int[G.num_nodes()];
        return true;
    }

    virtual bool run() {
        sssp_nolength(G, dist, root);
        return true;
    }

    virtual bool post_process() {
        //---------------------------------
        // dist values of 10 nodes
        //---------------------------------
        for (int i = 0; i < 10; i++) {
            printf("dist[%d] = %d\n", i, dist[i]);
        }
        return true;
    }
};

int main(int argc, char** argv) {
    my_main M;
    M.main(argc, argv);
}
