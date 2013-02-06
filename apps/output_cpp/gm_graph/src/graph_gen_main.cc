#include "gm_graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <assert.h>
#include <sys/time.h>

#include "graph_gen.h"

//  CREATE RMAT  or random file and dump
int main(int argc, char** argv) {

    //-----------------------------
    // create RMAT graph
    //-----------------------------
    if (argc < 5) {
        printf("%s <Num Node> <Num Edge> <out filename> <0~1>\n", argv[0]);
        printf("\t 0: uniform random (multigprah)\n");
        printf("\t 1: uniform random alternative (multigraph)\n");
        printf("\t 2: uniform random \n");
        printf("\t 3: uniform random (multigraph - xorshift random)\n");
        //        printf("\t 3: RMAT random (mu\n");
        exit(0);
    }

    node_t N = (node_t) atoll(argv[1]);
    edge_t M = (edge_t) atoll(argv[2]);
    int gtype = atoi(argv[4]);
    if (N == 0) {printf("Empty graph not allowed\n"); return EXIT_FAILURE;}
    printf("Creating Graph, N = %lld, M = %lld, Type = %d\n", (int64_t)N, (int64_t) M, gtype);


    gm_graph* g;
    int random_seed = 1997;

    struct timeval T1, T2;
    gettimeofday(&T1, NULL);

    switch (gtype) {
        case 0:
            g = create_uniform_random_graph(N, M, random_seed, false);
            break;
        case 1:
            g = create_uniform_random_graph2(N, M, random_seed);
            break;
        case 2:
            g = create_uniform_random_nonmulti_graph(N, M, random_seed);
            break;
        case 3:
            g = create_uniform_random_graph(N, M, random_seed, true);
            break;
        /*
        case 4:
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
    return EXIT_SUCCESS;
}

