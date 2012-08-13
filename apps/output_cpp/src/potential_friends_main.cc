#include "common_main.h"
#include "potential_friends.h"

class my_main: public main_t
{
public:
    gm_property_of_collection_impl<gm_node_set, false>* coll;

    virtual ~my_main() {
    }

    virtual bool prepare() {
        return true;
    }

    virtual bool run() {
        coll = new gm_property_of_collection_impl<gm_node_set, false>(G.num_nodes());
        potential_friends(G, *coll);
        return true;
    }

    virtual bool post_process() {
        printf("potential friends for the first 10 nodes (max. 10 entries per node shown):\n");
        for (node_t iter_aux = 0; iter_aux < 10 && iter_aux < G.num_nodes(); iter_aux++) {
            gm_node_set x = (*coll)[iter_aux];
            gm_node_set::seq_iter x0_I = x.prepare_seq_iteration();
            if (!x0_I.has_next()) continue;
            printf("node #%d: {", iter_aux);
            int i = 0;
            while (x0_I.has_next()) {
                printf("%d", x0_I.get_next());
                if (++i == 10) {
                    printf("...");
                    break;
                } else if (x0_I.has_next()) {
                    printf(", ");
                }
            }
            printf("}\n");
        }
        return true;
    }
};

int main(int argc, char** argv) {
    my_main M;
    M.main(argc, argv);
}
