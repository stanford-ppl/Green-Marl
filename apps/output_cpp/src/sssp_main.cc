#include "common_main.h"
#include "sssp.h"
#include "gm_rand.h"

class my_main: public main_t
{
public:
    int32_t* len; // length of each edge
    int32_t* dist;  // distance of each node
    node_t root;

    virtual ~my_main() {
        delete[] len;
        delete[] dist;
    }

    //--------------------------------------------------------
    // create 4 groups randomly
    //--------------------------------------------------------
    virtual bool prepare() {
	gm_rand32 xorshift_rng;
        root = 0;
        dist = new int[G.num_nodes()];
        len = new int[G.num_edges()];
        for (int i = 0; i < G.num_edges(); i++)
            len[i] = (xorshift_rng.rand() % 100) + 1;  // length: 1 ~ 100
        return true;
    }

    virtual bool run() {
        sssp(G, dist, len, root);
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
