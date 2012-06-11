
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
        communities(G, com);
        return true;
    }

    virtual bool post_process() {
     	for(int i = 0; i < G.num_nodes(); i++) {
			//printf("Node: %d\t Com: %d\n", i, com[i]);
		}
      return true;
    }
};

int main(int argc, char** argv)
{
    my_main M;
    M.main(argc, argv);
}

