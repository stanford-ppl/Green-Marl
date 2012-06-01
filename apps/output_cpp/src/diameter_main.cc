
#include "common_main.h"
#include "diameter.h"

class my_main: public main_t
{
public:
    int* com; // 

    //--------------------------------------------------------
    // create 4 groups randomly
    //--------------------------------------------------------
    virtual bool prepare() {
        com = new int[G.num_nodes()];
        return true;
    }

	int result;

    virtual bool run() {
        result = diameter(G);
        return true;
    }

    virtual bool post_process() {
      printf("Diameter: %d\n", result);
      return true;
    }
};

int main(int argc, char** argv)
{
    my_main M;
    M.main(argc, argv);
}

