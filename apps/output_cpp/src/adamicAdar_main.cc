#include "common_main.h"
#include "adamicAdar.h"  // defined in generated
class aa_main: public main_t
{
public:
    double* aa; // edge property
    int method;

    ~aa_main() {
        delete[] aa;
    }

    aa_main() {
        method = 0;
        aa = NULL;
    }

    virtual bool prepare() {
        aa = new double[G.num_edges()];
        return true;
    }

    virtual bool run() {
        //if (method == 0)
            adamicAdar(G, aa);
        //else
        //    adamicAdar2(G, aa);
        return true;
    }

    virtual void print_arg_info() {
        //printf("[usemethod=0/1]");
    }

    virtual bool check_args(int argc, char** argv) {
        if (argc > 0) method = atoi(argv[0]);
        return true;
    }

    virtual bool post_process() {
        int max_cnt = 0;
        for (int i = 0; i < G.num_edges(); i++) {
            if (aa[i] != 0) {
                printf("%d-> %5.5f\n", i, aa[i]);
                if (max_cnt++ == 100) break;
            }
        }
        return true;
    }
};

int main(int argc, char** argv) {
    aa_main M;
    M.main(argc, argv);
}

