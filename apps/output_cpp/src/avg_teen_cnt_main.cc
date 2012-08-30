#include "common_main.h"
#include "avg_teen_cnt.h"  // defined in generated
class my_main: public main_t
{
public:
    int* age;
    int* teen_cnt;
    int K;
    float avg;
    my_main() {
        K = 5;
    }

    virtual bool prepare() {
        age = new int[G.num_nodes()];
        #pragma omp paralel for
        for (int i = 0; i < G.num_nodes(); i++) {
        	age[i] = 10;
        }
        teen_cnt = new int[G.num_nodes()];
        return true;
    }

    virtual bool run() {
        avg = avg_teen_cnt(G, age, teen_cnt, K);
        return true;
    }

    virtual bool post_process() {
        //---------------------------------
        // values
        //---------------------------------
        printf("avg = %0.9lf\n", avg);
        delete[] age;
        delete[] teen_cnt;
        return true;
    }

    virtual void print_arg_info() {
        printf("[K=5]");
    }

    virtual bool check_args(int argc, char** argv) {
        if (argc > 0) {
            K = atoi(argv[0]);
            if (K <= 0) return false;
        }

        return true;
    }

};

int main(int argc, char** argv) {
    my_main M;
    M.main(argc, argv);
}
