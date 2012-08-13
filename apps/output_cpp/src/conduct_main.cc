#include "common_main.h"
#include "conduct.h"
#include "gm_rand.h"

class my_main: public main_t
{
public:
    int32_t* membership;
    double C;

    ~my_main() {
        delete[] membership;
    }

    //--------------------------------------------------------
    // create 4 groups randomly
    //--------------------------------------------------------
    virtual bool prepare() {
        membership = new int32_t[G.num_nodes()];
	gm_rand32 xorshift_rng;
        for (int i = 0; i < G.num_nodes(); i++) {
	    int32_t r = xorshift_rng.rand() % 100;
            if (r < 10)
                membership[i] = 0;  // 10%
            else if (r < (10 + 20))
                membership[i] = 1;  // 20%
            else if (r < (10 + 20 + 30))
                membership[i] = 2;  // 30%
            else
                membership[i] = 3;  // 40%
        }
        return true;
    }

    virtual bool run() {
        C = 0;
        for (int i = 0; i < 4; i++)
            C += conduct(get_graph(), membership, i);

        return true;
    }

    virtual bool post_process() {
        //---------------------------------
        // values
        //---------------------------------
        printf("sum C = %lf\n", C);
        return true;
    }
};

int main(int argc, char** argv) {
    my_main M;
    M.main(argc, argv);
}
