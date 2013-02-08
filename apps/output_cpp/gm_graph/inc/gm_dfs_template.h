#ifndef GM_DFS_TEMPLATE_H
#define GM_DFS_TEMPLATE_H
#include <omp.h>
#include <string.h>
#include <set>
#include <vector>
#include "gm_graph.h"

//-----------------------------------------------
// template for DFS
//  Note that recursion-base DFS will surely  crash due to 
//  stack overflow, when applied to small-world graphs.
//  (It will visit O(N) nodes before ever pop up)
//  Thus, here we implement DFS withour recursion.
//-----------------------------------------------

struct _dfs_state
{
    _dfs_state(node_t N, edge_t I, edge_t E) :
            node(N), idx(I), end(E) {
    }
    node_t node; // node 
    edge_t idx;  // edge idx
    edge_t end;    // 
};

template<bool has_pre_visit, bool has_post_visit, bool has_navigator, bool use_reverse_edge>
class gm_dfs_template
{
  protected:
    virtual void visit_pre(node_t t)=0;
    virtual void visit_post(node_t t)=0;
    virtual bool check_navigator(node_t t, edge_t idx)=0;

  public:
    gm_dfs_template(gm_graph& _G) :
            G(_G) {
        visited_bitmap = NULL; // bitmap
    }

    virtual ~gm_dfs_template() {
        delete visited_bitmap;
    }

    void prepare(node_t root_node) {
        root = root_node;
        cnt = 0;
        visited_small.clear();

        is_small = true;
        curr_node = INVALID_NODE;
        curr_idx = 0;
        curr_end = 0;
        THRESHOLD_LARGE = std::max((int)(G.num_nodes()*0.1), 4096); 
    }

    void do_dfs() {
        enter_node(root);
        main_loop();
    }

  private:
    void prepare_large() {
        delete[] visited_bitmap;

        visited_bitmap = new unsigned char[(G.num_nodes() + 7) / 8];

#pragma omp parallel for
        for (int i = 0; i < (G.num_nodes() + 7) / 8; i++)
            visited_bitmap[i] = 0;

        std::set<node_t>::iterator I;
        for (I = visited_small.begin(); I != visited_small.end(); I++) {
            node_t u = *I;
            _gm_set_bit(visited_bitmap, u);
        }
        is_small = false;
        stack.reserve(G.num_nodes());
    }

    void enter_node(node_t n) {
        // push current node
        _dfs_state S(curr_node, curr_idx, curr_end);
        stack.push_back(S);

        curr_node = n;
        curr_idx = (use_reverse_edge) ? G.r_begin[n] : G.begin[n];
        curr_end = (use_reverse_edge) ? G.r_begin[n + 1] : G.begin[n + 1];

        // mark visited
        add_visited(n);
        cnt++;
        if (cnt == THRESHOLD_LARGE)  // if go over threshold, it will probably visit all the nodes
                {
            prepare_large();
        }

        if (has_pre_visit) visit_pre(n);
    }

    void exit_node(node_t n) {
        if (has_post_visit) visit_post(n);
        _dfs_state S = stack.back();
        stack.pop_back();

        curr_node = S.node;
        curr_idx = S.idx;
        curr_end = S.end;
    }

    void main_loop() {
        //----------------------------------
        // Repeat until stack is empty
        //----------------------------------
        while (curr_node != INVALID_NODE) {
            //----------------------------------
            // Every neighbor has been visited
            //----------------------------------
            if (curr_idx == curr_end) {
                exit_node(curr_node);
                continue;
            }

            else {
                //----------------------------------
                // check every non-visited neighbor
                //----------------------------------
                node_t z;
                if (use_reverse_edge) {
                    z = G.r_node_idx[curr_idx];
                } else {
                    z = G.node_idx[curr_idx];
                }
                if (has_visited(z)) {
                    curr_idx++;
                    continue;
                }
                if (has_navigator) {
                    if (check_navigator(z, curr_idx) == false) {
                        curr_idx++;
                        continue;
                    }
                }
                curr_idx++;
                enter_node(z);
                continue;
            }
        }
    }

    void add_visited(node_t n) {
        if (is_small)
            visited_small.insert(n);
        else
            _gm_set_bit(visited_bitmap, n);
    }

    bool has_visited(node_t n) {
        if (is_small) {
            return (visited_small.find(n) != visited_small.end());
        } else {
            return _gm_get_bit(visited_bitmap, n);
        }
    }

  protected:
    node_t root;
    gm_graph& G;

    // stack implementation
    node_t stack_ptr;
    std::vector<_dfs_state> stack;
    node_t curr_node;
    edge_t curr_idx;
    edge_t curr_end;

    // visited set implementation
    node_t cnt;
    unsigned char* visited_bitmap;
    std::set<node_t> visited_small;
    bool is_small;
    int THRESHOLD_LARGE;
    static const node_t INVALID_NODE = -1;

};

#endif
