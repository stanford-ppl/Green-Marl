
typedef int node_t;
int gm_num_threads() {return 8;}
int gm_thread_id() {return 0;}

class gpgraph_t {
    public:
        int numNodes() {return n;}
        int* nodes;
        int* edges;
        int n;

};

static inline unsigned _gm_get_bit(unsigned char* Bit, int n) 
{
    int bit_pos = n / 8;
    int bit_loc = n % 8;
    unsigned val = (Bit[bit_pos] >> bit_loc) & 0x01;
    return val;
}

static inline void  _gm_set_bit(unsigned char* BitMap, int n) 
{
    int bit_pos = n / 8;
    int bit_loc = n % 8;
    unsigned char or_val = 0x1 << bit_loc;
    unsigned char org_val = BitMap[bit_pos];
    unsigned char new_val = or_val | org_val;
    BitMap[bit_pos] = new_val;
}

// true if I'm the one who set the bit
static inline bool _gm_set_bit_atomic(unsigned char* BitMap, int n) 
{
    int bit_pos = n / 8;
    int bit_loc = n % 8;
    unsigned char or_val = 0x1 << bit_loc;
    unsigned char old_val = __sync_fetch_and_or(&BitMap[bit_pos], or_val);
    if ( ((old_val >> bit_loc) & 0x01) == 0) return true;
    return false;
}


class _bfs_helper {

public:
    _bfs_helper() {
        graph = NULL;
        visited = NULL;
        next_Q = curr_Q = level = NULL;
        num_nodes = 0;
        num_threads = 0;
    }
    // calling sequence
    //  set_num_threads -> set_graph_size -> set_root

    // should be called before set_graph_size
    void set_num_threads(int p) {
        if (num_threads == p) return;
        else if (num_threads > 0)  {
            for(int i=0;i<num_threads;i++) {
                delete [] local_Q[i];
            }
            delete [] local_Q;
        }
        num_threads = p;
        local_Q = new node_t*[num_threads];
        for(int i=0;i<num_threads;i++)
            local_Q[i] = NULL;
    }

    void set_graph_size(int num_nodes) {
        if (this->num_nodes == num_nodes) return;
        delete [] next_Q;
        delete [] curr_Q;
        delete [] visited;
        delete [] level;
        for(int i=0;i<num_threads;i++)
            delete [] local_Q[i];

        BIT_END  = (numNodes + 7) / 8;
        visited  = new unsigned char[BIT_END];
        next_Q   = new node_t[numNodes];
        curr_Q   = new node_t[numNodes];
        level    = new unsigned short[numNodes];
        for(int i=0;i<num_threads;i++)
            local_Q[i] = new node_t[NumNodes / num_threads];
    }

    void set_root_node(node_t r)
    {
        cnt_curr = 0;
        cnt_next = 0;
        state = ST_SEQ;
        is_finished = false;
        curr_level = -1;

        _gm_setbit(visited, r);
        level[r] = 0;
        next_Q[cnt_next++] = r;
        choose_method();
    }

    ~ _bfs_helper() {
        delete [] visited;
        delete [] next_Q;
        delete [] curr_Q;
        delete [] level;
        for(int i=0;i<num_threads;i++) {
            delete [] local_Q[i];
        }
        delete [] local_Q;
    }

    inline bool add_next_seq(node_t n) {
        if (!_gm_get_bit(visited,n)) {
            _gm_set_bit(visited, n);
            next_Q[next_cnt++] = n;
            level[n] = curr_level + 1;
            return true;
        }
        return false;
    }

    inline bool add_next_local_q(node_t n, int tid, int& cnt) {
        if (!_gm_get_bit(visited,n)) {
            if (_gm_set_bit_atomic(visited, n)) {
                local_Q[cnt++] = n;
                level[n] = curr_level + 1;
                return true;
            }
        }
        return false;
    }

    inline bool add_next_level(node_t n, int& cnt) {
        if (!_gm_get_bit(visited,n)) {
            if (_gm_set_bit_atomic(visited, n)) {
                level[n] = curr_level + 1;
                return true;
            }
        }
        return false;
    }

    void choose_method() {
        if (cnt_next ==0) is_finished = true;
        else {
            switch(state) {
                case ST_SEQ: 
                case ST_QUE: 
                case ST_TO_QUE:
                    if      (cnt_next <= THRESHOLD1) state = ST_SEQ;
                    else if (cnt_next <= THRESHOLD2) state = ST_QUE;
                    else state = ST_TO_RD;
                    break;
                case ST_TO_RD:
                    if (cnt_next <= THRESHOLD2) state = ST_TO_QUE;
                    else state = ST_RD;
                    break;
                case ST_RD:
                    if (cnt_next <= (2* cnt_curr)) state = ST_TO_QUE;
                    else state = ST_RD;
                    break;
            }

            // ready for new curr level exploration
            cnt_curr = cnt_next;
            cnt_next = 0;
            node_t* temp = curr_Q;
            curr_Q = next_Q;
            next_Q = temp;
            curr_level ++;
        }
    }

public:
    static const int THRESHOLD1 = 128;
    static const int THRESHOLD2 = 4096;
    int numNodes;
    int BIT_END;
    int state;
    unsigned char* visited  ;
    node_t* next_Q          ; 
    node_t* curr_Q          ; 
    unsigned short* level   ;
    int cnt_curr            ;
    int cnt_next            ;
    bool is_finished ;
    int curr_level ;
    static const int ST_SEQ    = 0    ;
    static const int ST_QUE    = 1    ;
    static const int ST_TO_RD  = 2    ;
    static const int ST_RD     = 3    ;
    static const int ST_TO_QUE = 4    ;
};


int temp()
{
    int root = 0;
    gpgraph_t G;
    _bfs_helper _BFS;
    _BFS.set_num_threads(gm_num_threads());

    if ((G.nodes[root+1] - G.nodes[root]) == 0) {
        // hack

    } 
    else {
      // BFS
        _BFS.set_num_nodes(G.NumNodes());
        for(int i = 0; i < _BFS.BIT_END; i++)
            _BFS.visited[i] = 0;
        _BFS.set_root(r);

      // Level 0

        while (!_BFS.is_finished) {
            switch(_BFS.state) {
                case _bfs_helper::ST_QUE: {
                    int _local_cnt=0;
                    for(int i=0; i < _BFS.cnt_curr; i++) {
                        node_t m = _BFS.curr_Q[i];

                        // filter condition here

                        // user body here

                        // next level iteration
                        for(int j = G.nodes[m]; j < G.nodes[m+1];j++) {
                            int n = G.edges[j];
                            if (!_gm_get_bit(_BFS.visited, n)) {
                                if (_gm_set_bit_atomic(_BFS.visited, n)) {
                                    local_Q[gm_thread_id()][_local_cnt++] = n;
                                    _BFS.level[n] = _BFS.curr_level + 1;
                                }
                            }

                            // (merged) user body here
                            // e.g. down nbrs
                            if (_BFS.level[n] == _BFS.curr_level+1) { // next level
                                // ....

                            }
                        }
                    } 

                    int idx = __sync_fetch_and_add(&_BFS.cnt_next, _local_cnt); // increase next
                    memcpy( &(_BFS.nextQ[idx]), local_Q[gm_thread_id()], sizeof(node_t) * _local_cnt); // copy queue
                    break;
                }
            }

            _BFS.choose_method();
        }
    } // end of BFS
}
