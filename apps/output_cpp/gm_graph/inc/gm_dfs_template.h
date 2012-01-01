#include <omp.h>
#include <string.h>
#include "gm_graph.h"
#include "gm_set.h"

#ifndef GM_DFS_TEMPLATE_H
#define GM_DFS_TEMPLATE_H


template <
    bool has_pre_visit, 
    bool has_post_visit, 
    bool has_navigator,
    bool use_reverse_edge>
class gm_dfs_template
{

protected:
    virtual void visit_pre(node_t t)=0;
    virtual void visit_post(node_t t)=0;
    virtual bool check_navigator(node_t t)=0;

public:
gm_dfs_template(gm_graph& _G) : G(_G)
{
    visited = new gm_node_set(G.num_nodes());
}

virtual ~gm_dfs_template() 
{
    delete visited;
}

public:
void prepare(node_t root_node) 
{
    root = root_node;
    cnt = 0;
}

void do_dfs() 
{
    enter_node(root);

}

private:
inline void enter_node(node_t n)
{
    // mark visited
    visited->add(n);
    cnt++;
    if ((cnt ==(gm_node_set::THRESHOLD_UP+1)) && visited->is_small())
    {
        visited->migrate_representation();
    }

    if (has_pre_visit)
        visit_pre(n);

    //----------------------------------
    // check every non-visited neighbor
    //----------------------------------
    edge_t begin,end, i;
    if (use_reverse_edge) {
        begin = G.r_begin[n]; end = G.r_begin[n+1];
    } else {
        begin = G.begin[n]; end = G.begin[n+1];
    }
    for(i=begin; i<end;i++)
    {
        node_t z;
        if (use_reverse_edge) {
            z = G.r_node_idx[i];
        } else {
            z = G.node_idx[i];
        }

        // check visited
        if (visited->is_in(z)) continue;
        if (has_navigator) {
            if (check_navigator(z) == false) continue;
        }
        enter_node(z);
    }
    
    if (has_post_visit)
        visit_post(n);

}


protected:
    node_t root;
    gm_graph& G; 
    node_t cnt;
    gm_node_set* visited;
};

#endif
