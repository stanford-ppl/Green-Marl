#include "gm_graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <assert.h>

gm_graph* create_uniform_random_graph(index_t N, index_t M, int seed=1993,bool need_backedge=true);
gm_graph* create_RMAT_graph(index_t N, index_t M, int rseed=2387, bool need_gackedge = true, double a=0.45, double b=0.25, double c=0.15, bool permute=true);
bool test_backedge(gm_graph* g)
{
    int repeat = g->numNodes()*0.001;

    gm_graph&G = *g;
    for(int K = 0; K < repeat; K++)
    {
        int s = rand() % g->numNodes();
        for(int j=G.begin[s];j<G.begin[s+1];j++) {
            int n = G.node_idx[j];
            //printf("from %d, n = %d\n", s, n);
            bool found = false;
            for(int i=G.r_begin[n];i<G.r_begin[n+1];i++) {
                int m = G.r_node_idx[i];
                if (m==s) found = true;
            }
            if (!found) {
                printf("\n%d -> %d \n", s, n);
            }
            assert(found);
        }
    }


    return true;
}
//  CREATE RMAT  or random file and dump
int main(int argc, char** argv)
{

	//-----------------------------
    // create RMAT graph
	//-----------------------------
	if (argc < 5) {
		printf("%s <Num Node> <Num Edge> <out filename> <0~1>\n", argv[0]);
		printf("\t 0: uniform random\n");
		printf("\t 1: RMAT random\n");
		exit(0);
	}

	int N = atoi (argv[1]);
	int M = atoi (argv[2]);
    bool need_back_edge = true;
    int gtype = atoi(argv[4]);

	gm_graph* g;
    int random_seed = 1997;
    switch(gtype) 
    {
        case 0:
		    g = create_uniform_random_graph(N, M, random_seed, need_back_edge);
            break;
        case 1:
		    g = create_RMAT_graph(N, M, random_seed, need_back_edge);
            break;
    }

    test_backedge(g);
    printf("saving to file = %s\n", argv[3]);fflush(stdout);
	g->storeBinary(argv[3]);

	delete g;
}



