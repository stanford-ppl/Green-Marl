
#include "common_main.h"
#include "v_cover.h"

class my_main: public main_t
{
public:
    bool* selected;
    int covered;

    ~my_main() {
        delete[] selected;
    }

    //--------------------------------------------------------
    // create 4 groups randomly
    //--------------------------------------------------------
    virtual bool prepare() {
        selected = new bool[G.num_edges()];
        return true;
    }

    virtual bool run() {
        covered = v_cover(G, selected);
        return true;
    }

    virtual bool post_process() {
        //---------------------------------
        // values
        //---------------------------------
        printf("covered (may be non-deterministic) = %d\n", covered);
        return true;
    }
};

int main(int argc, char** argv) {
    my_main M;
    M.main(argc, argv);
}
