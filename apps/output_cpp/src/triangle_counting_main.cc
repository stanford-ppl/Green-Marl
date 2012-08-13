#include "common_main.h"
#include "triangle_counting.h"

class my_main: public main_t
{
public:
    int tCount;

    virtual bool prepare() {
        return true;
    }

    virtual bool run() {
        tCount = triangle_counting(G);
        return true;
    }

    virtual bool post_process() {
        printf("number of triangles: %d\n", tCount);
        return true;
    }
};

int main(int argc, char** argv) {
    my_main M;
    M.main(argc, argv);
}
