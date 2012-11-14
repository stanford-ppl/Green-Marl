#include <stdio.h>
#include "gm.h"

int32_t test_procedure(gm_graph& G, int a, double* np, double* ep)
{
#pragma omp parallel for
    for(node_t i = 0; i < G.num_nodes(); i++) {
        np[i] = np[i] * a;
    }

#pragma omp parallel for
    for(edge_t i = 0; i < G.num_edges(); i++) {
        ep[i] = ep[i] + a -a;
    }

    return 1;
}

int main(int argc, char** argv)
{
    gm_default_usermain Main;

    Main.declare_property("np", GMTYPE_DOUBLE, true, true, GM_NODEPROP);
    Main.declare_property("ep", GMTYPE_DOUBLE, true, true, GM_EDGEPROP);
    Main.declare_scalar("a", GMTYPE_INT, true, false);
    Main.declare_return(GMTYPE_INT);

    if (!Main.process_arguments(argc, argv))
        return EXIT_FAILURE;

    if (!Main.do_preprocess())
        return EXIT_FAILURE;

    Main.begin_usermain();
    Main.set_return_i(
        test_procedure(Main.get_graph(),
            *((int32_t*)Main.get_scalar("a")),
            (double*)Main.get_property("np"),
            (double*)Main.get_property("ep")
            ));
    Main.end_usermain();

    if (!Main.do_postprocess())
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
