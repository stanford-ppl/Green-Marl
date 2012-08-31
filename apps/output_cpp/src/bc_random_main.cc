#include "common_main.h"
#include "bc_random.h"  // defined in generated
#include "gm_rand.h"
class my_main: public main_t
{
public:
    float* BC;

    ~my_main() {
        delete[] BC;
    }

    my_main() {
        BC = NULL;
    }

    virtual bool prepare() {
        BC = new float[G.num_nodes()];
        return true;
    }

    virtual bool run() {
        bc_random(G, BC, 10);
        return true;
    }

    virtual bool post_process() {
        printf("BC[0] = %0.9lf\n", BC[0]);
        printf("BC[1] = %0.9lf\n", BC[1]);
        printf("BC[2] = %0.9lf\n", BC[2]);
        printf("BC[3] = %0.9lf\n", BC[3]);
        return true;
    }
};

int main(int argc, char** argv) {
    my_main M;
    M.main(argc, argv);
}
