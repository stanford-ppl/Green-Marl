#ifndef COMMON_MAIN_H
#define COMMON_MAIN_H

#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "gm_graph.h"
#include "gm_helper_functions.h"

class main_t 
{
  protected:
    gm_graph G;
    int method;
    int repeat;

  public:
    virtual void main(int argc, char** argv)
    {
        method = 0; 
        repeat = 0;
        if (argc < 3) {
            printf("%s <graph_name> <num_threads> (<repeat>)\n", argv[0]);
            exit(0);
        }
        time_to_exclude = 0;

        if (argc >= 5) {
            repeat = atoi(argv[4]);
        }

        //---------------------------------
        // Load graph
        //---------------------------------
        char *fname = argv[1];
        bool b = G.loadBinary(fname);
        if (!b) {
            printf("error reading graph\n");
            exit(0);
        }

        /*
        method = atoi(argv[2]);
        if ((method != 0) && (method!=1)) {
            printf("invalid method\n");
            exit(0);
        }
        */

        int num = atoi(argv[2]);
        //printf("num threads = %d\n", num);
        omp_set_num_threads(num);

        create_data();
        prepare();

        struct timeval T1, T2;
        gettimeofday(&T1, NULL); 
        run();
        gettimeofday(&T2, NULL); 
        printf("time=%lf\n", 
            (T2.tv_sec - T1.tv_sec)*1000 + (T2.tv_usec - T1.tv_usec)*0.001 
            - time_to_exclude
        );


#ifdef CHECK_ANSWER
        check_answer();
#endif
        cleanup();
    }

    virtual bool check_answer() { return true; }
    virtual bool run() = 0;
    virtual bool prepare() { return true;}
    virtual bool create_data() = 0;
    virtual bool cleanup() {return true;}

protected:
    double time_to_exclude;

};

#endif
