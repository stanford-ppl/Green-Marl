
#include "common_main.h"
#include "sssp.h"

class my_main: public main_t
{
public:
    int* len; // length of each edge
    int* dist;  // distance of each node
    node_t root;

    //--------------------------------------------------------
    // create 4 groups randomly
    //--------------------------------------------------------
    virtual bool prepare() {
        root = 0;
        dist = new int[G.num_nodes()];
        len = new int[G.num_edges()];
        for(int i=0;i<G.num_edges();i++) 
            len[i] = (rand() % 100) + 1;  // length: 1 ~ 100
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
        for(int i=0;i<10;i++) {
            printf("dist[%d] = %d\n", i, dist[i]);
        }
        return true;
    }
};

int main(int argc, char** argv)
{
    my_main M;
    M.main(argc, argv);
}
