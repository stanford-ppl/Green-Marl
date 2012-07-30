#include "common_main.h"
#include "kosaraju.h"  // defined in generated

class my_main: public main_t
{
public:
    int* membership;
    int  num_membership;

    virtual ~my_main() {
        delete[] membership;
    }

    my_main() : membership(NULL), num_membership(0) {
    }

    virtual bool prepare() {
        membership = new int[G.num_nodes()];
        num_membership = 0;
        return true;
    }

    virtual bool run() 
    {
        num_membership = kosaraju(G, membership);
        return true;
    }

    virtual bool post_process() {
        printf("num_membership = %d\n", num_membership);
        return true;
    }
};

#define TEST_LARGE  1

int main(int argc, char** argv)
{
#if TEST_LARGE
    my_main M;
    M.main(argc, argv);
#else

   gm_graph G;
   for(int i=0;i<10;i++)
        G.add_node();

    G.add_edge(0,1);
    G.add_edge(1,2);
    G.add_edge(2,3);
    G.add_edge(3,0);
    G.add_edge(5,8);
    G.add_edge(7,4);
    G.add_edge(9,1);
    G.add_edge(1,4);
    G.add_edge(2,5);
    G.add_edge(4,9);

    int* membership;
    int  num_membership;

    membership = new int[G.num_nodes()];
    num_membership = 0;

    num_membership = kosaraju(G, membership);
    printf("num_membership = %d\n", num_membership);
#endif
}

