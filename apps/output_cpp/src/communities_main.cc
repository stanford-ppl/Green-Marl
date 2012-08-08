#include "common_main.h"
#include "communities.h"

class my_main: public main_t
{
public:

    node_t* comm;

    virtual ~my_main() {
        delete[] comm;
    }

    virtual bool prepare() {
        comm = new node_t[G.num_nodes()];
        return true;
    }

    virtual bool run() {
        communities(G, comm);
        return true;
    }

    virtual bool post_process() {
        int* commCount = new int[G.num_nodes()];
        for(int i = 0; i < G.num_nodes(); i++)
            commCount[comm[i]]++;
        printf("Community\t#Nodes\t\t(showing max 10 entries)\n");
        for(int i = 0, x = 0; x < 10 && i < G.num_nodes(); i++)
            if(commCount[i] > 0) {
                printf("%d\t\t%d\n", i, commCount[i]);
                x++;
            }
        delete[] commCount;
        return true;
    }
};

int main(int argc, char** argv) {
    my_main M;
    M.main(argc, argv);
}
