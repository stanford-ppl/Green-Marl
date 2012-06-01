
#include "common_main.h"
#include "conduct.h"

class my_main: public main_t
{
public:
    int32_t* membership;
    double C;

    //--------------------------------------------------------
    // create 4 groups randomly
    //--------------------------------------------------------
    virtual bool prepare() {
        membership = new int32_t[G.num_nodes()];
        for(int i=0;i<G.num_nodes();i++)
        {
            float f = drand48();
            if (f < 0.1) 
                membership[i] = 0;  // 10%
            else if (f < (0.1+0.2)) 
                membership[i] = 1;  // 20%
            else if (f < (0.1+0.2+0.3)) 
                membership[i] = 2;  // 30%
            else
                membership[i] = 3;  // 40%
        }
        return true;
    }

    virtual bool run() {
        C = 0;
        for(int i = 0; i < 4; i++)
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

int main(int argc, char** argv)
{
    my_main M;
    M.main(argc, argv);
}
