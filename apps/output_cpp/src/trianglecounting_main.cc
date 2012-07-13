#include "common_main.h"
#include "trianglecounting.h"

class my_main: public main_t
{
public:

    virtual ~my_main() {}

    virtual bool prepare() {
        return true;
    }

    virtual bool run() {
        gm_property_of_collection_impl<gm_node_set, false> coll(G.num_nodes());
        int tCount = TriangleCounting(G);
        PotentialFriends(G, coll);
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
