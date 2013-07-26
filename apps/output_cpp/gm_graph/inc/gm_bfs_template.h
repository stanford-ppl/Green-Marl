#ifndef GM_BFS_TEMPLATE_H
#define GM_BFS_TEMPLATE_H
#include <omp.h>
#include <string.h>
#include <map>
#include <set>

#include "gm_graph.h"
#include "gm_atomic_wrapper.h"
#include "gm_bitmap.h"


// todo: consideration for non small-world graph
template<typename level_t, bool use_multithread, bool has_navigator, bool use_reverse_edge, bool save_child>
class gm_bfs_template
{

  public:
    gm_bfs_template(gm_graph& _G) :
            G(_G) {
        visited_bitmap = NULL; // bitmap
        visited_level = NULL;

        //global_curr_level = NULL;
        //global_next_level = NULL;
        //global_queue = NULL;
        thread_local_next_level = NULL;
        down_edge_array = NULL;
        down_edge_set = NULL;
        if (save_child) {
            down_edge_set = new std::set<edge_t>();
        }
    }

    virtual ~gm_bfs_template() {
        delete [] visited_bitmap;
        delete [] visited_level;
        //delete [] global_queue;
        delete [] thread_local_next_level;
        delete [] down_edge_set;
        delete [] down_edge_array;
    }

    void prepare(node_t root_node, int max_num_thread) {
        int num_thread;
        if (use_multithread) {
            num_thread = max_num_thread;
        } else {
            num_thread = 1;
        }

        is_finished = false;
        curr_level = 0;
        root = root_node;
        state = ST_SMALL;
        assert(root != gm_graph::NIL_NODE);

        //global_queue = new node_t[G.num_nodes()];
        //global_curr_level = global_queue;
        //global_next_level = NULL;

        // create local queues
        thread_local_next_level = new std::vector<node_t>[num_thread];
        for (int i = 0; i < num_thread; i++)
            thread_local_next_level[i].reserve(G.num_nodes()/num_thread/2);

    }

    void do_bfs_forward() {
        //---------------------------------
        // prepare root node
        //---------------------------------
        curr_level = 0;
        curr_count = 0;
        next_count = 0;

        small_visited[root] = curr_level;
        curr_count++;
        global_vector.push_back(root);
        global_curr_level_begin = 0;
        global_next_level_begin = curr_count;

        level_count.push_back(curr_count);
        level_queue_begin.push_back(global_curr_level_begin);

#define PROFILE_LEVEL_TIME  0
#if PROFILE_LEVEL_TIME
        struct timeval T1, T2;
#endif

        bool is_done = false;
        while (!is_done) {
#if PROFILE_LEVEL_TIME
            gettimeofday(&T1,NULL);
            printf("state = %d, curr_count=%d, ", state, curr_count);
#endif
            switch (state) {
                case ST_SMALL: {
                    for (node_t i = 0; i < curr_count; i++) {
                        node_t t = global_vector[global_curr_level_begin + i];
                        iterate_neighbor_small(t);
                        visit_fw(t);            // visit after iteration. in that way, one can check  down-neighbors quite easily
                    }
                    break;
                }
                case ST_QUE: {
                    if (use_multithread)  // do it in parallel
                    {
                        #pragma omp parallel
                        {
                            int tid = omp_get_thread_num();
                            #pragma omp for nowait schedule(dynamic,128)
                            for (node_t i = 0; i < curr_count; i++) {
                                //node_t t = global_curr_level[i];
                                node_t t = global_vector[global_curr_level_begin + i];
                                iterate_neighbor_que(t, tid);
                                visit_fw(t);
                            }
                            finish_thread_que(tid);
                        }
                    }
                    else { // do it in sequential
                            int tid = 0;
                            for (node_t i = 0; i < curr_count; i++) {
                                //node_t t = global_curr_level[i];
                                node_t t = global_vector[global_curr_level_begin + i];
                                iterate_neighbor_que(t, tid);
                                visit_fw(t);
                            }
                            finish_thread_que(tid);
                    }
                    break;
                }
                case ST_Q2R: {
                    if (use_multithread) {  // do it in parallel
                        #pragma omp parallel
                        {
                            node_t local_cnt = 0;
                            #pragma omp for nowait schedule(dynamic,128)
                            for (node_t i = 0; i < curr_count; i++) {
                                //node_t t = global_curr_level[i];
                                node_t t = global_vector[global_curr_level_begin + i];
                                iterate_neighbor_rd(t, local_cnt);
                                visit_fw(t);
                            }
                            finish_thread_rd(local_cnt);
                        }
                    } else { // do it sequentially
                            node_t local_cnt = 0;
                            for (node_t i = 0; i < curr_count; i++) {
                                //node_t t = global_curr_level[i];
                                node_t t = global_vector[global_curr_level_begin + i];
                                iterate_neighbor_rd(t, local_cnt);
                                visit_fw(t);
                            }
                            finish_thread_rd(local_cnt);
                    }
                    break;
                }

                case ST_RD: {
                    if (use_multithread) { // do it in parallel
                        #pragma omp parallel
                        {
                            node_t local_cnt = 0;
                            #pragma omp for nowait schedule(dynamic,128)
                            for (node_t t = 0; t < G.num_nodes(); t++) {
                                if (visited_level[t] == curr_level) {
                                    iterate_neighbor_rd(t, local_cnt);
                                    visit_fw(t);
                                }
                            }
                            finish_thread_rd(local_cnt);
                        }
                    } else { // do it in sequential
                            node_t local_cnt = 0;
                            for (node_t t = 0; t < G.num_nodes(); t++) {
                                if (visited_level[t] == curr_level) {
                                    iterate_neighbor_rd(t, local_cnt);
                                    visit_fw(t);
                                }
                            }
                            finish_thread_rd(local_cnt);
                    }
                    break;
                }
                case ST_R2Q: {
                    if (use_multithread) { // do it in parallel
                        #pragma omp parallel
                        {
                            int tid = omp_get_thread_num();
                            #pragma omp for nowait schedule(dynamic,128)
                            for (node_t t = 0; t < G.num_nodes(); t++) {
                                if (visited_level[t] == curr_level) {
                                    iterate_neighbor_que(t, tid);
                                    visit_fw(t);
                                }
                            }
                            finish_thread_que(tid);
                        }
                    } else {
                            int tid = 0;
                            for (node_t t = 0; t < G.num_nodes(); t++) {
                                if (visited_level[t] == curr_level) {
                                    iterate_neighbor_que(t, tid);
                                    visit_fw(t);
                                }
                            }
                            finish_thread_que(tid);
                    }
                    break;
                }

                // reverse read
                case ST_RRD: {
                    #pragma omp parallel if (use_multithread)
                    {
                        node_t local_cnt = 0;
                        #pragma omp for nowait schedule(dynamic,128)
                        for (node_t t = 0; t < G.num_nodes(); t++) {
                            if (visited_level[t] == curr_level) {
                                visit_fw(t);
                            }
                            else if (visited_level[t] == __INVALID_LEVEL) {
                                if (check_parent_rrd(t)) {
                                    local_cnt ++;
                                    _gm_set_bit(visited_bitmap, t);
                                    visited_level[t] = (curr_level + 1);
                                }
                            }
                        }
                        finish_thread_rd(local_cnt);
                    }
                    break;
                }
                             
                // reverse read2Q
                case ST_RR2Q: {
                    #pragma omp parallel if (use_multithread)
                    {

                        int tid = omp_get_thread_num();
                        #pragma omp for nowait schedule(dynamic,128)
                        for (node_t t = 0; t < G.num_nodes(); t++) {
                            if (visited_level[t] == curr_level) {
                                visit_fw(t);
                            }
                            else if (visited_level[t] == __INVALID_LEVEL) {
                                if (check_parent_rrd(t)) {
                                    _gm_set_bit(visited_bitmap, t);
                                    visited_level[t] = (curr_level + 1);
                                    thread_local_next_level[tid].push_back(t);
                                }
                            }
                        }
                        finish_thread_que(tid);
                    }
                    break;
                }

            } // end of switch

            do_end_of_level_fw();
            is_done = get_next_state();
#if PROFILE_LEVEL_TIME
            gettimeofday(&T2, NULL);
            printf("time = %6.5f ms\n", (T2.tv_sec - T1.tv_sec)*1000 + (T2.tv_usec - T1.tv_usec)*0.001);
#endif
        } // end of while
        
    }

    void do_bfs_reverse() {
        // This function should be called only after do_bfs_foward has finished.
        // assumption: small-world graph
        level_t& level = curr_level;
        while (true) {
            node_t count = level_count[level];
            //node_t* queue_ptr = level_start_ptr[level];
            node_t* queue_ptr;
            node_t begin_idx = level_queue_begin[level];
            if (begin_idx == -1) { 
                queue_ptr = NULL;
            } else {
                queue_ptr = & (global_vector[begin_idx]);
            }
           
            if (queue_ptr == NULL) {
#pragma omp parallel if (use_multithread)
                {
#pragma omp for nowait schedule(dynamic,128)
                    for (node_t i = 0; i < G.num_nodes(); i++) {
                        if (visited_level[i] != curr_level) continue;
                        visit_rv(i);
                    }
                }
            } else {
#pragma omp parallel if (use_multithread)
                {
#pragma omp for nowait
                    for (node_t i = 0; i < count; i++) {
                        node_t u = queue_ptr[i];
                        visit_rv(u);
                    }
                }
            }

            do_end_of_level_rv();
            if (level == 0) break;
            level--;
        }
    }

    bool is_down_edge(edge_t idx) {
        if (state == ST_SMALL)
            return (down_edge_set->find(idx) != down_edge_set->end());
        else
            return down_edge_array[idx];
    }

  protected:
    virtual void visit_fw(node_t t)=0;
    virtual void visit_rv(node_t t)=0;
    virtual bool check_navigator(node_t t, edge_t nx)=0;
    virtual void do_end_of_level_fw() {
    }
    virtual void do_end_of_level_rv() {
    }

    node_t get_root() {
        return root;
    }

    level_t get_level(node_t t) {
        // GCC expansion
        if (__builtin_expect((state == ST_SMALL), 0)) {
            if (small_visited.find(t) == small_visited.end())
                return __INVALID_LEVEL;
            else
                return small_visited[t];
        } else {
            return visited_level[t];
        }
    }

    level_t get_curr_level() {
        return curr_level;
    }


  private:
    bool get_next_state() {
        const char* state_name[5] = {"SMALL","QUEUE","Q2R","RD","R2Q"};

        if (next_count == 0) return true;  // BFS is finished

        int next_state = state;
        bool already_expanded = false;
        const float RRD_THRESHOLD = 0.05;
        switch (state) {
            case ST_SMALL:
                if (next_count >= THRESHOLD1) {
                    prepare_que();
                    next_state = ST_QUE;
                }
                break;
            case ST_QUE:
               if (already_expanded) break;

               if (next_count >= G.num_nodes()*RRD_THRESHOLD)  
               {
                   already_expanded = true;
                    prepare_read();
                    if (!save_child && G.has_reverse_edge()) 
                    {
                        next_state = ST_RRD;
                    }
                    else
                        next_state = ST_RD;
                    break;
                }
                else if ((next_count >= THRESHOLD2) && (next_count >= curr_count*5)) {
                   already_expanded = true;
                    prepare_read();
                    next_state = ST_Q2R;
                }
                break;
            case ST_Q2R:
                if (!save_child && G.has_reverse_edge() && (next_count >= G.num_nodes()*RRD_THRESHOLD))
                {
                    next_state = ST_RRD;
                }
                else
                    next_state = ST_RD;
                break;
            case ST_RD:
                if (next_count >= G.num_nodes()*RRD_THRESHOLD) {
                    next_state = ST_RRD;
                }
                else if (next_count <= (2 * curr_count)) {
                    next_state = ST_R2Q;
                }
                break;
            case ST_R2Q:
                next_state = ST_QUE;
                break;
            case ST_RRD:
                if (next_count <= (2 * curr_count)) {
                    next_state = ST_RR2Q;
                }
                break;
            case ST_RR2Q:
                next_state = ST_QUE;
                break;
        }

        finish_level(state);
        state = next_state;

        return false;
    }

    void finish_level(int state) {
        if ((state == ST_RD) || (state == ST_Q2R)) {
            // output queue is not valid
        } else { // move output queue
            //node_t* temp = &(global_next_level[next_count]);
            //global_curr_level = global_next_level;
            //global_next_level = temp;
            global_curr_level_begin = global_next_level_begin;
            global_next_level_begin = global_next_level_begin + next_count;
        }

        curr_count = next_count;
        next_count = 0;
        curr_level++;

        // save 'new current' level status
        level_count.push_back(curr_count);
        if ((state == ST_RD) || (state == ST_Q2R) || (state == ST_RRD)) {
            //level_start_ptr.push_back(NULL);
            level_queue_begin.push_back(-1);
        } else {
            //level_start_ptr.push_back(global_curr_level);
            level_queue_begin.push_back(global_curr_level_begin);
        }
    }

    void get_range(edge_t& begin, edge_t& end, node_t t) {
        if (use_reverse_edge) {
            begin = G.r_begin[t];
            end = G.r_begin[t + 1];
        } else {
            begin = G.begin[t];
            end = G.begin[t + 1];
        }
    }
    void get_range_rev(edge_t& begin, edge_t& end, node_t t) {
        if (use_reverse_edge) {
            begin = G.begin[t];
            end = G.begin[t + 1];
        } else {
            begin = G.r_begin[t];
            end = G.r_begin[t + 1];
        }
    }

    node_t get_node(edge_t& n) {
        if (use_reverse_edge) {
            return G.r_node_idx[n];
        } else {
            return G.node_idx[n];
        }
    }
    node_t get_node_rev(edge_t& n) {
        if (use_reverse_edge) {
            return G.node_idx[n];
        } else {
            return G.r_node_idx[n];
        }
    }

    void iterate_neighbor_small(node_t t) {
        edge_t begin;
        edge_t end;
        get_range(begin, end, t);
        for (edge_t nx = begin; nx < end; nx++) {
            node_t u = get_node(nx);

            // check visited
            if (small_visited.find(u) == small_visited.end()) {
                if (has_navigator) {
                    if (check_navigator(u, nx) == false) continue;
                }

                if (save_child) {
                    save_down_edge_small(nx);
                }

                small_visited[u] = curr_level + 1;
                //global_next_level[next_count++] = u;
                global_vector.push_back(u); 
                next_count++;
            }
            else if (save_child) {
                if (has_navigator) {
                    if (check_navigator(u, nx) == false) continue;
                }

                if (small_visited[u] == (curr_level+1)){
                    save_down_edge_small(nx);
                }
            }
        }
    }

    // should be used only when save_child is enabled
    void save_down_edge_small(edge_t idx) {
        down_edge_set->insert(idx);
    }

    void save_down_edge_large(edge_t idx) {
        down_edge_array[idx] = 1;
    }

    void prepare_que() {

        global_vector.reserve(G.num_nodes());

        // create bitmap and edges
        visited_bitmap = new unsigned char[(G.num_nodes() + 7) / 8];
        visited_level = new level_t[G.num_nodes()];
        if (save_child) {
            down_edge_array = new unsigned char[G.num_edges()];
        }

        if (use_multithread) {
#pragma omp parallel
            {
#pragma omp for nowait //schedule(dynamic,65536)
                for (node_t i = 0; i < (G.num_nodes() + 7) / 8; i++)
                    visited_bitmap[i] = 0;

#pragma omp for nowait //schedule(dynamic,65536)
                for (node_t i = 0; i < G.num_nodes(); i++)
                    visited_level[i] =  __INVALID_LEVEL;

                if (save_child) {
#pragma omp for nowait //schedule(dynamic,65536)
                    for (edge_t i = 0; i < G.num_edges(); i++)
                        down_edge_array[i] = 0;
                }
            }
        } else {
            for (node_t i = 0; i < (G.num_nodes() + 7) / 8; i++)
                visited_bitmap[i] = 0;
            for (node_t i = 0; i < G.num_nodes(); i++)
                visited_level[i] =  __INVALID_LEVEL;
            if (save_child) {
                for (edge_t i = 0; i < G.num_edges(); i++)
                    down_edge_array[i] = 0;
            }
        }

        typename std::map<node_t, level_t>::iterator II;
        for (II = small_visited.begin(); II != small_visited.end(); II++) {
            node_t u = II->first;
            level_t lev = II->second;
            _gm_set_bit(visited_bitmap, u);
            visited_level[u] = lev;
        }

        if (save_child) {
            typename std::set<edge_t>::iterator J;
            for (J = down_edge_set->begin(); J != down_edge_set->end(); J++) {
                down_edge_array[*J] = 1;
            }
        }
    }

    void iterate_neighbor_que(node_t t, int tid) {
        edge_t begin;
        edge_t end;
        get_range(begin, end, t);
        for (edge_t nx = begin; nx < end; nx++) {
            node_t u = get_node(nx);

            // check visited bitmap
            // test & test& set
            if (_gm_get_bit(visited_bitmap, u) == 0) {
                if (has_navigator) {
                    if (check_navigator(u, nx) == false) continue;
                }

                bool re_check_result;
                if (use_multithread) {
                    re_check_result = _gm_set_bit_atomic(visited_bitmap, u);
                } else {
                    re_check_result = true;
                    _gm_set_bit(visited_bitmap, u);
                }

                if (save_child) {
                    save_down_edge_large(nx);
                }

                if (re_check_result) {
                    // add to local q
                    thread_local_next_level[tid].push_back(u);
                    visited_level[u] = (curr_level + 1);
                }
            }
            else if (save_child) {
                if (has_navigator) {
                    if (check_navigator(u, nx) == false) continue;
                }
                if (visited_level[u] == (curr_level +1)) {
                    save_down_edge_large(nx);
                }
            }
        }
    }

    void finish_thread_que(int tid) {
        node_t local_cnt = thread_local_next_level[tid].size();
        //copy curr_cnt to next_cnt
        if (local_cnt > 0) {
            node_t old_idx = _gm_atomic_fetch_and_add_node(&next_count, local_cnt);
            // copy to global vector
            memcpy(&(global_vector[global_next_level_begin + old_idx]), 
                   &(thread_local_next_level[tid][0]), 
                   local_cnt * sizeof(node_t));
        }
        thread_local_next_level[tid].clear();
    }

    void prepare_read() {
        // nothing to do
    }

    void iterate_neighbor_rd(node_t t, node_t& local_cnt) {
        edge_t begin;
        edge_t end;
        get_range(begin, end, t);
        for (edge_t nx = begin; nx < end; nx++) {
            node_t u = get_node(nx);

            // check visited bitmap
            // test & test& set
            if (_gm_get_bit(visited_bitmap, u) == 0) {
                if (has_navigator) {
                    if (check_navigator(u, nx) == false) continue;
                }

                bool re_check_result;
                if (use_multithread) {
                    re_check_result = _gm_set_bit_atomic(visited_bitmap, u);
                } else {
                    re_check_result = true;
                    _gm_set_bit(visited_bitmap, u);
                }

                if (save_child) {
                    save_down_edge_large(nx);
                }

                if (re_check_result) {
                    // add to local q
                    visited_level[u] = curr_level + 1;
                    local_cnt++;
                }
            }
            else if (save_child) {
                if (has_navigator) {
                    if (check_navigator(u, nx) == false) continue;
                }
                if (visited_level[u] == (curr_level +1)) {
                    save_down_edge_large(nx);
                }
            }
        }
    }

    void finish_thread_rd(node_t local_cnt) {
        _gm_atomic_fetch_and_add_node(&next_count, local_cnt);
    }

    // return true if t is next level
    bool check_parent_rrd(node_t t)
    {
        bool ret = false;
        edge_t begin;
        edge_t end;
        get_range_rev(begin, end, t);

        if (has_navigator) {
           // [XXX] fixme 
           if (check_navigator(t, 0) == false) return false;
        }

        for (edge_t nx = begin; nx < end; nx++) {
            node_t u = get_node_rev(nx);
            //if (_gm_get_bit(visited_bitmap, u) == 0) continue;
            if (visited_level[u] == (curr_level )) 
                return true;
        }
        return false;
    }


    //-----------------------------------------------------
    //-----------------------------------------------------
    static const int ST_SMALL = 0;
    static const int ST_QUE = 1;
    static const int ST_Q2R = 2;
    static const int ST_RD = 3;
    static const int ST_R2Q = 4;
    static const int ST_RRD = 5;
    static const int ST_RR2Q = 6;
    static const int THRESHOLD1 = 128;  // single threaded
    static const int THRESHOLD2 = 1024; // move to RD-based

    // not -1. 
    //(why? because curr_level-1 might be -1, when curr_level = 0)
    static const level_t __INVALID_LEVEL = -2;

    int state;

    unsigned char* visited_bitmap; // bitmap
    level_t* visited_level; // assumption: small_world graph
    bool is_finished;
    level_t curr_level;
    node_t root;
    gm_graph& G;
    node_t curr_count;
    node_t next_count;

    std::map<node_t, level_t> small_visited;
    std::set<edge_t>* down_edge_set;
    unsigned char* down_edge_array;

    //node_t* global_next_level;
    //node_t* global_curr_level;
    //node_t* global_queue;
    std::vector<node_t> global_vector; 
    node_t global_curr_level_begin;
    node_t global_next_level_begin;

    //std::vector<node_t*> level_start_ptr;
    std::vector<node_t> level_queue_begin;
    std::vector<node_t> level_count;

    std::vector<node_t>* thread_local_next_level;
};

#endif
