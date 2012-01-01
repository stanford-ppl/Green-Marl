
#include <stdio.h>
#include <stdlib.h>
#include "gm_graph.h"
#include "gm_helper_functions.h"

gm_graph::gm_graph()
{
	begin =  NULL;
	node_idx =  NULL;
	r_begin =  NULL;
	r_node_idx =  NULL;
    _numNodes =0;
    _numEdges =0;
    _reverse_edge = false;
    _frozen = false;
    _directed = false;
}

gm_graph::~gm_graph()
{
    delete [] node_idx;
    delete [] begin;
    delete [] r_begin;
    delete [] r_node_idx;
}

void gm_graph::freeze()
{
    if (_frozen) return;

    node_t n_nodes = num_nodes();
    edge_t n_edges = num_edges();

    allocate_memory(n_nodes, n_edges, _reverse_edge);

    // iterate over graph and make new structure
    edge_t next = 0;
    for(node_t i = 0; i< n_nodes; i++)
    {
        std::vector<node_t>& Nbrs = flexible_graph[i];
        begin[i] = next;

        // find 
        std::vector<node_t>::iterator I;
        for(I=Nbrs.begin(); I!=Nbrs.end();I++)
        {
            node_idx[next] = *I;
            next++;
        }
    }
    begin[n_nodes] = next;

    if (_reverse_edge)
    {
        edge_t next = 0;
        for(node_t i = 0; i< n_nodes; i++)
        {
            std::vector<node_t>& Nbrs = flexible_reverse_graph[i];
            r_begin[i] = next;

            // find 
            std::vector<node_t>::iterator I;
            for(I=Nbrs.begin(); I!=Nbrs.end();I++)
            {
                r_node_idx[next] = *I;
                next++;
            }
        }
        r_begin[n_nodes] = next;
    }

    // free flexible structure
    flexible_graph.clear();
    flexible_reverse_graph.clear();

    _frozen = true;
}

void gm_graph::thaw()
{
    if (!_frozen) return;
    node_t n_nodes = num_nodes();
    
    flexible_graph.clear();

    // vect<vect> ==> CSR
    for(node_t i = 0; i< n_nodes; i++)
    {
        flexible_graph[i].clear();
        flexible_graph[i].reserve(begin[i+1] - begin[i]);
        for(edge_t e = begin[i]; e < begin[i+1]; e++)
        {
            node_t dest = node_idx[e];
            flexible_graph[i].push_back(dest);
        }
    }

    if (_reverse_edge) {
        for(node_t i = 0; i< n_nodes; i++)
        {
            edge_t cnt = 0;
            flexible_graph[i].clear();
            flexible_graph[i].reserve(r_begin[i+1] - r_begin[i]);
            for(edge_t e = r_begin[i]; e < r_begin[i+1]; e++)
            {
                node_t dest = r_node_idx[e];
                flexible_graph[i].push_back(dest);
            }
        }
    }

    delete [] begin;
    delete [] node_idx;
    delete [] r_begin;
    delete [] r_node_idx;
    begin = node_idx = r_begin = r_node_idx = NULL;

    _frozen = false;
}


void gm_graph::remove_reverse_edges() 
{
    if (_frozen) {
        flexible_reverse_graph.clear();
    } else {
        delete [] r_begin;
        delete [] r_node_idx;
        r_begin = NULL;
        r_node_idx = NULL;
    }
    _reverse_edge = false;
}

void gm_graph::make_reverse_edges() 
{
    if (_reverse_edge) return;

    printf("creating reverse edges....\n");

    node_t n_nodes = num_nodes();

    if (_frozen) {
        delete [] r_begin;
        delete [] r_node_idx;

        r_begin = new edge_t[num_nodes() +1];
        r_node_idx = new node_t[num_edges()];

        edge_t* temp = new edge_t[num_nodes()];

        // iterate over foward edges but create reverse edges

        // step 1 count in-degree
        for(node_t i = 0; i< n_nodes; i++)
        {
            temp[i] = 0;
        }
        for(node_t i = 0; i< n_nodes; i++)
        {
            for(edge_t e = begin[i]; e < begin[i+1]; e++)
            {
                node_t dest = node_idx[e];
                temp[dest]++;
            }
        }

        // step 2 set r_begin
        edge_t next = 0;
        for(node_t i = 0; i< n_nodes; i++)
        {
            r_begin[i] = next;
            next += temp[i];
        }
        r_begin[n_nodes] = num_edges();

        // step 3 set r_node_idx
        for(node_t i = 0; i< n_nodes; i++)
        {
            for(edge_t e = begin[i]; e < begin[i+1]; e++)
            {
                node_t dest = node_idx[e];
                edge_t ptr = r_begin[dest] + temp[dest] - 1;
                temp[dest] --;
                r_node_idx[ptr] = i;
            }
        }
        for(node_t i = 0; i< n_nodes; i++)
        {
            assert(temp[i] == 0);
        }

        delete [] temp;

    } else {
        
        // iterate over foward graph and create reverse edges
        flexible_reverse_graph.clear();
        for(node_t i = 0; i< n_nodes; i++)
        {
            std::vector<node_t>& Nbrs = flexible_graph[i];
            std::vector<node_t>::iterator I;
            for(I=Nbrs.begin(); I!=Nbrs.end();I++)
            {
                node_t dest = *I;
                flexible_reverse_graph[dest].push_back(i);
            }
        }
    }

    _reverse_edge = true;
    printf(".... finished \n");
}

void gm_graph::prepare_external_creation(node_t n, edge_t m, bool reverse_edge)
{
    clear_graph();
    allocate_memory(n, m, reverse_edge);
    _frozen = true;
}
void gm_graph::allocate_memory(node_t n, edge_t m, bool reverse_edge)
{
    delete [] begin;
    delete [] node_idx;
    delete [] r_begin;
    delete [] r_node_idx;

    _reverse_edge = reverse_edge;

	begin =  new edge_t[n+1];
	node_idx =  new node_t[m];
    if (reverse_edge)
    {
	    r_begin =  new edge_t[n+1];
	    r_node_idx =  new node_t[m];
    }
    else {
	    r_begin =  NULL;
	    r_node_idx = NULL;
    }
    _numNodes = n;
    _numEdges = m;
}


node_t gm_graph::add_node() {
    if (_frozen) thaw();


    std::vector<node_t> T;  // empty vector
    flexible_graph[_numNodes] = T; // T is copied

    if (_reverse_edge)
        flexible_reverse_graph[_numNodes] = T;  // T is copied

    _numNodes++;
    return _numNodes;
}

edge_t gm_graph::add_edge(node_t n, node_t m)
{
    assert(is_node(n));
    assert(is_node(m));

    if (_frozen) thaw();

    flexible_graph[n].push_back(m);

    if (_reverse_edge) {
        flexible_reverse_graph[m].push_back(n);
    }

    _numEdges++;
    return _numEdges;
}

void gm_graph::remove_edge(node_t n, node_t m)
{
    assert(is_node(n));
    assert(is_node(m));
    if (_frozen) thaw();

    std::vector<node_t>& nbrs = flexible_graph[n];
    edge_t cnt = 0; // count of deleted edges

    // compress neighbor list ( mark and copy-back)
    for(edge_t i=0;i<nbrs.size(); i++) {
        node_t dest = nbrs[i];
        if (dest == m) {
            nbrs[i] = NIL_NODE;  // (mark)
            cnt ++;
        }
    }
    edge_t back = 0;
    for(edge_t i=0;i<nbrs.size();i++) {
        if (nbrs[i] == NIL_NODE) {
            back++;
        }
        else nbrs[i - back] = nbrs[i]; // (copy back)
    }
    assert(back == cnt);
    nbrs.resize( nbrs.size() - cnt);

    // [xxx] problem: edge id becomes duplicated
    _numEdges -= back;

    // remove back-edges
    if (_reverse_edge) {
        cnt = 0;
        std::vector<node_t>& nbrs = flexible_reverse_graph[m];

        for(edge_t i=0;i<nbrs.size(); i++) {
            node_t dest = nbrs[i];
            if (dest == n) {
                nbrs[i] = NIL_NODE;
                cnt ++;
            }
        }
        edge_t back = 0;
        for(edge_t i=0;i<nbrs.size();i++) {
            if (nbrs[i] == NIL_NODE) {
                back++;
            }
            else nbrs[i - back] = nbrs[i];
        }
        assert(back == cnt);
        nbrs.resize( nbrs.size() - cnt);
    }
}


#define MAGIC_WORD	0x10102048
bool gm_graph::store_binary(char* filename)
{
    if (!_frozen)
        freeze();

	FILE *f = fopen(filename, "wb");
	if (f == NULL) {
		printf("cannot open %s for writing\n", filename);
		return false;
	}

	printf("storing data\n");

	// write it 4B wise?
	uint32_t key = MAGIC_WORD;
	fwrite(&key, 4, 1, f);

    key = sizeof(edge_t);
    fwrite(&key, 4, 1, f); // index size (4B)
    key = sizeof(node_t);
    fwrite(&key, 4, 1, f);  // node size (4B)

    uint32_t need_back = (this->_reverse_edge) ? 1 : 0;
	fwrite(&need_back, 4, 1, f);

	
	key = this->_numNodes;
	fwrite(&key, sizeof(edge_t), 1, f);

	key = this->_numEdges;
	fwrite(&key, sizeof(edge_t), 1, f);

	for(int i=0;i<_numNodes+1; i++) {
		key = this->begin[i];
		fwrite(&key, sizeof(edge_t), 1, f);
	}

	for(int i=0;i<_numEdges; i++) {
		key = this->node_idx[i];
		fwrite(&key, sizeof(node_t), 1, f);
	}

    if (need_back) {
	    for(int i=0;i<_numNodes+1; i++) {
		    key = this->r_begin[i];
		    fwrite(&key, sizeof(edge_t), 1, f);
	    }
	    for(int i=0;i<_numEdges; i++) {
		    key = this->r_node_idx[i];
		    fwrite(&key, sizeof(node_t), 1, f);
	    }
    }

	fclose(f);
    return true;
}

void gm_graph::clear_graph()
{
    flexible_graph.clear();
    flexible_reverse_graph.clear();

    delete [] begin;
    delete [] node_idx;
    delete [] r_begin;
    delete [] r_node_idx;

    begin = node_idx = r_begin = r_node_idx = NULL;

    _numNodes = 0;
    _numEdges = 0;
    _frozen = false;
}


bool gm_graph::load_binary(char* filename)
{
    clear_graph();
    bool need_back;
	uint32_t key;
    int i;

	FILE *f = fopen(filename, "rb");
	if (f == NULL) {
		printf("cannot open %s for reading\n", filename);
        goto error_return;
	}

	// write it 4B wise?
	i = fread(&key, 4, 1, f);
	if ((i!=1) || (key != MAGIC_WORD)) {
		printf("wrong file format, KEY mismatch: %d, %x\n", i, key);
        goto error_return;
	}


    i = fread(&key, 4, 1, f); // index size (4B)
    if (key!= sizeof(edge_t)) {
        printf("edge_t size mismatch:%d (expect %ld)\n", key, sizeof(edge_t));
        goto error_return;
    }
    i = fread(&key, 4, 1, f); // index size (4B)
    if (key!= sizeof(node_t)) {
        printf("node_t size mismatch:%d (expect %ld)\n", key, sizeof(node_t));
        goto error_return;
    }

    //---------------------------------------------
    // need back, numNodes, numEdges
    //---------------------------------------------
    edge_t N, M;
	i = fread(&key, 4, 1, f);
	if (i!=1) {
		printf("Error reading need-back from file \n");
        goto error_return;
	}
    need_back = key? true : false;
	i = fread(&N, sizeof(edge_t), 1, f);
	if (i!=1) {
		printf("Error reading numNodes from file \n");
        goto error_return;
	}
	i = fread(&M, sizeof(edge_t), 1, f);
	if (i!=1) {
		printf("Error reading numEdges from file \n");
        goto error_return;
    }
	

	//printf("N = %ld, M = %ld\n", (long)N, (long)M);
    allocate_memory(N, M, need_back);
	
	for(edge_t i=0;i<N+1; i++) {
		int k = fread(&key, sizeof(edge_t), 1, f);
		if ((k!=1)) {
			printf("Error reading node begin array\n");
            goto error_return;
		}
		this->begin[i] = key;
	}

	for(edge_t i=0;i<M; i++) {
		int k = fread(&key, sizeof(node_t), 1, f);
		if ((k!=1)) {
			printf("Error reading edge-end array\n");
            goto error_return;
		}
		this->node_idx[i] = key;
    }

    if (need_back) {
	    for(edge_t i=0;i<N+1; i++) {
		    int k = fread(&key, sizeof(edge_t), 1, f);
		    if ((k!=1)) {
			    printf("Error reading node begin array, tranposed\n");
                goto error_return;
            }
		    this->r_begin[i] = key;
		}
	    for(edge_t i=0;i<M; i++) {
		    int k = fread(&key, sizeof(node_t), 1, f);
		    if ((k!=1)) {
			    printf("Error reading edge-end array, transposed\n");
                goto error_return;
		    }
		    this->r_node_idx[i] = key;
        }
	}

	fclose(f);
    _frozen = true;
    return true;

error_return:
    clear_graph();
    return false;
}
