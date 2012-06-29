#include "common_main.h"
#include "pagerank.h"  // defined in generated
class my_main: public main_t
{
public:
    double* rank;
    int max_iter;
    double e;
    double d;
    my_main() {
        e = 0.001;
        d = 0.85;
        max_iter = 100;
    }

    virtual bool prepare() {
        rank = new double[G.num_nodes()];
        return true;
    }

    virtual bool run() {
        pagerank(G, e, d, max_iter, rank);
        return true;
    }

    virtual bool post_process() {
        //---------------------------------
        // values
        //---------------------------------
        printf("rank[0] = %0.9lf\n", rank[0]);
        printf("rank[1] = %0.9lf\n", rank[1]);
        printf("rank[2] = %0.9lf\n", rank[2]);
        printf("rank[3] = %0.9lf\n", rank[3]);
        delete[] rank;
        return true;
    }

    virtual void print_arg_info() {
        printf("[max_iteration=100] [eplision=0.001] [dela=0.85]");
    }

    virtual bool check_args(int argc, char** argv) {
        if (argc > 0) {
            max_iter = atoi(argv[0]);
            if (max_iter <= 0) return false;
        }
        if (argc > 1) {
            e = atof(argv[1]);
            if (e <= 0) return false;
        }
        if (argc > 2) {
            d = atof(argv[2]);
            if (d <= 0) return false;
        }

        return true;
    }

};

int main(int argc, char** argv) {
    my_main M;
    M.main(argc, argv);
}
