
#include "common_main.h"
#include "shortestpath.h"
#include <time.h>

class my_main: public main_t
{
public:

    gm_node_order* path;
    node_t source;
    node_t destination;

    virtual bool prepare() {
        return true;
    }

    virtual bool run() {
	srand(time(NULL));
	source = rand() % G.num_nodes();
	destination = rand() % G.num_nodes();
	path = new gm_node_order(G.num_nodes());
        shortestpath(G, source, destination, *path);
        return true;
    }

    virtual bool post_process() {
	printf("Shortest path: source: %d\tdestination: %d\n", source, destination);
	
	gm_node_order::seq_iter II = path->prepare_seq_iteration();
	printf("%d ", II.get_next());
	while(II.has_next()) {
	    node_t node = II.get_next();
	    printf("-> %d ", node);
	}
	printf("\n");
        return true;
    }
};

int main(int argc, char** argv)
{
    my_main M;
    M.main(argc, argv);
}
