
#include "pagerank.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
typedef int32_t attr_id_t;

#include "common_main.h"
//void PageRank_Ref(gm_graph& G, double e, double d, int32_t max, double* PR);
class my_main: public main_t
{
public:
    double* rank;

    virtual bool create_data() {
        rank = new double[G.numNodes()];
        return true;
    }

    virtual bool run() {
        double e = 0.001;
        double d = 0.85;
        int max = 100;
        if (method == 0) {
            PageRank(G, e,d, max, rank);
        }
        else {
            assert(false);
            //PageRank_Ref(G, e,d, max, rank);
        }
        return true;
    }

    virtual bool check_answer() {
        //---------------------------------
        // values
        //---------------------------------
        printf("rank[0] = %0.9lf\n", rank[0]);
        printf("rank[1] = %0.9lf\n", rank[1]);
        printf("rank[2] = %0.9lf\n", rank[2]);
        printf("rank[3] = %0.9lf\n", rank[3]);
    }
};

int main(int argc, char** argv)
{
    my_main M;
    M.main(argc, argv);
}
