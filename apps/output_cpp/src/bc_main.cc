#include "common_main.h"
#include "bc.h"  // defined in generated
class my_main: public main_t
{
public:
    gm_node_seq* Seeds;

    float* BC;

    my_main() {
        Seeds = NULL;
    }

    virtual bool prepare() {
        Seeds = new gm_node_seq();
        BC = new float[G.num_nodes()];
        return true;
    }

    virtual bool run() {
        assert(Seeds != NULL);
        // pick 5 random starting points;
        for (int i = 0; i < 5; i++) {
            node_t t;
            do {
                t = rand();
            } while (t >= G.num_nodes());

            Seeds->push_back(t);
        }

        comp_BC(G, BC, *Seeds);
        return true;
    }

    virtual bool post_process() {
        printf("BC[0] = %0.9lf\n", BC[0]);
        printf("BC[1] = %0.9lf\n", BC[1]);
        printf("BC[2] = %0.9lf\n", BC[2]);
        printf("BC[3] = %0.9lf\n", BC[3]);

        delete[] BC;
        delete Seeds;
        return true;
    }
};

int main(int argc, char** argv) {
    my_main M;
    M.main(argc, argv);
}
