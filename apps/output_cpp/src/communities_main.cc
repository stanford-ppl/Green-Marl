
#include "common_main.h"
#include "communities.h"

class my_main: public main_t
{
public:
    int* com; // 

    //--------------------------------------------------------
    // create 4 groups randomly
    //--------------------------------------------------------
    virtual bool prepare() {
        com = new int[G.num_nodes()];
        return true;
    }

    virtual bool run() {

//	for(int i = 0; i < G.num_nodes(); i++) {
//		for (edge_t n0_idx = G.begin[i];n0_idx < G.begin[i+1] ; n0_idx ++) {
//			node_t n0 = G.node_idx [n0_idx];
//			printf("%d -> %d\n", i, n0);
//		}
//	}

        communities(G, com);
        return true;
    }

    virtual bool post_process() {
        //---------------------------------
        // dist values of 10 nodes
        //---------------------------------
     	for(int i = 0; i < G.num_nodes(); i++) {
		printf("Node: %d\t Com: %d\n", i, com[i]);
	}
      return true;
    }
};

int main(int argc, char** argv)
{
    my_main M;
    M.main(argc, argv);
}

