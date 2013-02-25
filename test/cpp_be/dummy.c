#include "gm.h"

#include "output/b52.h"
int main()
{
// nothing

#if 0
// test b52.cc
    { 
        gm_graph G;
        for(int i = 0; i < 6;i++)
            G.add_node();
        G.add_edge(0,1);
        G.add_edge(0,3);
        G.add_edge(0,2);
        G.add_edge(2,4);
        G.add_edge(3,4);
        G.add_edge(3,5);

        G.freeze();
        int e_values[6];
        e_values[0] = 10;
        e_values[1] = 20;
        e_values[2] = 30;
        e_values[3] = 40;
        e_values[4] = 50;
        e_values[5] = 60;

        b52(G, e_values);

        printf("semi sort\n");
        G.do_semi_sort();

        b52(G, e_values);

    }
#endif



#if 0
    {
        gm_graph G;

        std::vector<VALUE_TYPE> vprop_schema;
        vprop_schema.push_back(GMTYPE_DOUBLE);

        std::vector<VALUE_TYPE> eprop_schema;
        eprop_schema.push_back(GMTYPE_INT);

        std::vector<void* > v_props;
        std::vector<void* > e_props;
        G.load_adjacency_list("test.adj", 
                vprop_schema, eprop_schema,
                v_props, e_props, " \t", false);

        int* e_val = (int*) (e_props[0]);
        b52(G, e_val);
    }

#endif
}
