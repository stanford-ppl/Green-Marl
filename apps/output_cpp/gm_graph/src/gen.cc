#include "gm_graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <assert.h>
#include <sys/time.h>

gm_graph* create_uniform_random_graph(node_t N, edge_t M, long seed);
gm_graph* create_uniform_random_graph2(node_t N, edge_t M, long seed);
gm_graph* create_uniform_random_nonmulti_graph(node_t N, edge_t M, long seed);
/*
 gm_graph* create_RMAT_graph(node_t N, edge_t M, int rseed=2387, bool need_gackedge = true, double a=0.45, double b=0.25, double c=0.15, bool permute=true);
 */

bool test_backedge(gm_graph* g) {
    int repeat = g->num_nodes() * 0.001;

    gm_graph&G = *g;
    for (int K = 0; K < repeat; K++) {
        int s = rand() % g->num_nodes();
        for (int j = G.begin[s]; j < G.begin[s + 1]; j++) {
            int n = G.node_idx[j];
            //printf("from %d, n = %d\n", s, n);
            bool found = false;
            for (int i = G.r_begin[n]; i < G.r_begin[n + 1]; i++) {
                int m = G.r_node_idx[i];
                if (m == s) found = true;
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
int main(int argc, char** argv) {

    //-----------------------------
    // create RMAT graph
    //-----------------------------
    if (argc < 5) {
        printf("%s <Num Node> <Num Edge> <out filename> <0~1>\n", argv[0]);
        printf("\t 0: uniform random (multigprah)\n");
        printf("\t 1: uniform random alternative (multigprah)\n");
        printf("\t 2: uniform random \n");
        //        printf("\t 3: RMAT random (mu\n");
        exit(0);
    }

    node_t N = atol(argv[1]);
    edge_t M = atol(argv[2]);
    int gtype = atoi(argv[4]);

    gm_graph* g;
    int random_seed = 1997;

    struct timeval T1, T2;
    gettimeofday(&T1, NULL);

    switch (gtype) {
        case 0:
            g = create_uniform_random_graph(N, M, random_seed);
            break;
        case 1:
            g = create_uniform_random_graph2(N, M, random_seed);
            break;
        case 2:
            g = create_uniform_random_nonmulti_graph(N, M, random_seed);
            break;
            /*
             case 3:
             g = create_RMAT_graph(N, M, random_seed, need_back_edge);
             break;
             */
        default:
            printf("UNKNOWN GRAPH TYPE\n");
            exit(-1);
    }
    gettimeofday(&T2, NULL);
    printf("creation time (ms) = %lf\n", ((T2.tv_sec) - (T1.tv_sec)) * 1000 + (T2.tv_usec - T1.tv_usec) * 0.001);

    printf("saving to file = %s\n", argv[3]);
    fflush (stdout);
    gettimeofday(&T1, NULL);
    g->store_binary(argv[3]);
    gettimeofday(&T2, NULL);
    printf("storing time (ms) = %lf\n", ((T2.tv_sec) - (T1.tv_sec)) * 1000 + (T2.tv_usec - T1.tv_usec) * 0.001);

    delete g;
}

