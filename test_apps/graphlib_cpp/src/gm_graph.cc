
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
    _back_edge = false;
}

gm_graph::~gm_graph()
{
    delete [] node_idx;
    delete [] begin;
    delete [] r_begin;
    delete [] r_node_idx;
}

void gm_graph::allocate_memory(index_t n, index_t m, bool back_edge)

{
    delete [] begin;
    delete [] node_idx;
    delete [] r_begin;
    delete [] r_node_idx;
    _back_edge = back_edge;

	begin =  new index_t[n+1];
	node_idx =  new node_t[m];
    if (back_edge)
    {
	    r_begin =  new index_t[n+1];
	    r_node_idx =  new node_t[m];
    }
    else {
	    r_begin =  NULL;
	    r_node_idx = NULL;
    }
    _numNodes = n;
    _numEdges = m;
}


#include <stdint.h>

#define MAGIC_WORD	0x10102048
bool gm_graph::storeBinary(char* filename)
{
	FILE *f = fopen(filename, "wb");
	if (f == NULL) {
		printf("cannot open %s for writing\n", filename);
		return false;
	}

	printf("storing data\n");

	// write it 4B wise?
	uint32_t key = MAGIC_WORD;
	fwrite(&key, 4, 1, f);

    key = sizeof(index_t);
    fwrite(&key, 4, 1, f); // index size (4B)
    key = sizeof(node_t);
    fwrite(&key, 4, 1, f);  // node size (4B)

    uint32_t need_back = (this->_back_edge) ? 1 : 0;
	fwrite(&need_back, 4, 1, f);

	
	key = this->_numNodes;
	fwrite(&key, sizeof(index_t), 1, f);

	key = this->_numEdges;
	fwrite(&key, sizeof(index_t), 1, f);

	for(int i=0;i<_numNodes+1; i++) {
		key = this->begin[i];
		fwrite(&key, sizeof(index_t), 1, f);
	}

	for(int i=0;i<_numEdges; i++) {
		key = this->node_idx[i];
		fwrite(&key, sizeof(node_t), 1, f);
	}

    if (need_back) {
	    for(int i=0;i<_numNodes+1; i++) {
		    key = this->r_begin[i];
		    fwrite(&key, sizeof(index_t), 1, f);
	    }
	    for(int i=0;i<_numEdges; i++) {
		    key = this->r_node_idx[i];
		    fwrite(&key, sizeof(node_t), 1, f);
	    }
    }

	fclose(f);
    return true;
}

bool gm_graph::loadBinary(char* filename)
{
	FILE *f = fopen(filename, "rb");
	if (f == NULL) {
		printf("cannot open %s for reading\n", filename);
		return false ;
	}

	// write it 4B wise?
	uint32_t key;
	int i = fread(&key, 4, 1, f);
	if ((i!=1) || (key != MAGIC_WORD)) {
		printf("wrong file format, KEY mismatch: %d, %x\n", i, key);
		return false;
	}


    i = fread(&key, 4, 1, f); // index size (4B)
    if (key!= sizeof(index_t)) {
        printf("index_t size mismatch:%d (expect %ld)\n", key, sizeof(index_t));
        return false;
    }
    i = fread(&key, 4, 1, f); // index size (4B)
    if (key!= sizeof(node_t)) {
        printf("node_t size mismatch:%d (expect %ld)\n", key, sizeof(node_t));
        return false;
    }

    //---------------------------------------------
    // need back, numNodes, numEdges
    //---------------------------------------------
    index_t N, M;
	i = fread(&key, 4, 1, f);
	if (i!=1) {
		printf("Error reading need-back from file \n");
		return false;
	}
    bool need_back = key? true : false;
	i = fread(&N, sizeof(index_t), 1, f);
	if (i!=1) {
		printf("Error reading numNodes from file \n");
		return false;
	}
	i = fread(&M, sizeof(index_t), 1, f);
	if (i!=1) {
		printf("Error reading numEdges from file \n");
		return false;
	}

	//printf("N = %ld, M = %ld\n", (long)N, (long)M);
    allocate_memory(N, M, need_back);
	
	for(index_t i=0;i<N+1; i++) {
		int k = fread(&key, sizeof(index_t), 1, f);
		if ((k!=1)) {
			printf("Error reading node begin array\n");
			return false;
		}
		this->begin[i] = key;
	}

	for(index_t i=0;i<M; i++) {
		int k = fread(&key, sizeof(node_t), 1, f);
		if ((k!=1)) {
			printf("Error reading edge-end array\n");
			return false;
		}
		this->node_idx[i] = key;
    }

    if (need_back) {
	    for(index_t i=0;i<N+1; i++) {
		    int k = fread(&key, sizeof(index_t), 1, f);
		    if ((k!=1)) {
			    printf("Error reading node begin array, tranposed\n");
			    return false;
            }
		    this->r_begin[i] = key;
		}
	    for(index_t i=0;i<M; i++) {
		    int k = fread(&key, sizeof(node_t), 1, f);
		    if ((k!=1)) {
			    printf("Error reading edge-end array, transposed\n");
			    return false;
		    }
		    this->r_node_idx[i] = key;
        }
	}

	fclose(f);
    return true;
}
