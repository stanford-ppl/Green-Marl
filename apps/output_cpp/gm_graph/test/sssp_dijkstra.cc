#include "sssp_dijkstra.h"

bool dijkstra(gm_graph& G, double* G_Len, 
    node_t& root, node_t& dest, 
    node_t*G_Parent, edge_t*G_ParentEdge)
{
    //Initializations
    gm_rt_initialize();
    G.freeze();

    gm_mutatable_priority_map_unordered_min<node_t, double> Reachable( 0.0);
    //gm_mutatable_priority_map_simple_min<node_t, double> Reachable( 0.0);
    bool found = false ;
    bool failed = false ;
    bool* G_Reached = gm_rt_allocate_bool(G.num_nodes(),gm_rt_thread_id());

    Reachable.setValue_seq(root, (float)(0.000000));
    found = false ;
    failed = false ;

    #pragma omp parallel for
    for (node_t t0 = 0; t0 < G.num_nodes(); t0 ++) 
    {
        G_Parent[t0] = gm_graph::NIL_NODE ;
        G_Reached[t0] = false ;
    }
    while ( !found &&  !failed)
    {
        if (Reachable.size() == 0)
        {
            failed = true ;
        }
        else 
        {
            node_t next;

            next = Reachable.getMinKey_seq() ;
            if (next == dest)
            {
                found = true ;
            }
            else 
            {
                double dist = 0.0 ;

                G_Reached[next] = true ;
                dist = Reachable.getMinValue_seq() ;
                Reachable.removeMinKey_seq();
                for (edge_t v_idx = G.begin[next];v_idx < G.begin[next+1] ; v_idx ++) 
                {
                    node_t v = G.node_idx [v_idx];
                    if ( !G_Reached[v])
                    {
                        edge_t e;

                        e = v_idx ;
                        if ( !Reachable.hasKey_seq(v))
                        {
                            Reachable.setValue_seq(v, dist + G_Len[e]);
                            G_Parent[v] = next ;
                            G_ParentEdge[v] = e ;
                        }
                        else if (Reachable.getValue(v) > dist + G_Len[e])
                        {
                            Reachable.setValue_seq(v, dist + G_Len[e]);
                            G_Parent[v] = next ;
                            G_ParentEdge[v] = e ;
                        }
                    }
                }
            }
        }
    }
    gm_rt_cleanup();
    return  !failed; 
}

double get_path(gm_graph& G, node_t& begin0, 
    node_t& end, node_t* G_prev_node, 
    edge_t* G_prev_edge, double* G_edge_cost, 
    gm_node_seq& Q)
{
    //Initializations
    gm_rt_initialize();
    G.freeze();

    double total_cost = 0.0 ;
    node_t n;

    total_cost = (float)(0.000000) ;
    n = end ;
    if (G_prev_node[end] != gm_graph::NIL_NODE)
    {
        while (n != begin0)
        {
            edge_t e;

            Q.push_front(n);
            e = G_prev_edge[n] ;
            total_cost = total_cost + G_edge_cost[e] ;
            n = G_prev_node[n] ;
        }
    }
    return total_cost; 
}


