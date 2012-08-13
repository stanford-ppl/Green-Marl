#include "common_main.h"
#include "triangle_counting.h"

class my_main: public main_t
{
public:

    virtual ~my_main() {}

    virtual bool prepare() {
        return true;
    }

    virtual bool run() {
        int tCount = triangle_counting(G);

        printf("triangle count: %d\n", tCount);

        return true;
    }

    virtual bool post_process() {
        return true;
    }
};

int main(int argc, char** argv) {
    my_main M;
    M.main(argc, argv);
}
