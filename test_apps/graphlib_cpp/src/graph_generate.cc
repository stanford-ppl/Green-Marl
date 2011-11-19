
#include <stdlib.h>
#include <set>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "gm_graph.h"

gm_graph* create_uniform_random_graph(index_t N, index_t M, index_t seed, bool need_back)
{
    printf("need_back = %d\n", need_back?1:0);;;;
	// any random seed
	srand(seed);

   	gm_graph* g = new gm_graph();
   	g->allocate_memory(N, M, need_back);

	index_t* src = new index_t[M];
	index_t* dest = new index_t[M];
	index_t* degree = new index_t[N];
    index_t* in_degree = NULL;
	memset(degree, 0, sizeof(index_t)*N);
    if (need_back) {
	    in_degree = new index_t[N];
	    memset(in_degree, 0, sizeof(index_t)*N);
    }

	for(index_t i =0; i < M; i++) {
		src[i] = rand() % N;
		dest[i] = rand() % N;
        if (src[i] == dest[i]) {i=i-1; continue;}
		degree[src[i]]++;
        if (need_back)
		    in_degree[dest[i]]++;
	}

	g->begin[0] = 0;
    if (need_back) g->r_begin[0] = 0;
	for(index_t i=1; i <=N; i++) {
		g->begin[i] = g->begin[i-1] + degree[i-1];
        if (need_back) {
		    g->r_begin[i] = g->r_begin[i-1] + in_degree[i-1];
        }
    }

    for(int i=0; i < N; i++) {
        assert((g->begin[i+1] - g->begin[i]) == degree[i]);
        assert((g->r_begin[i+1] - g->r_begin[i]) == in_degree[i]);
    }

	for(index_t i=0; i <M; i++) {
		index_t u = src[i];
		index_t v = dest[i];

		index_t pos = degree[u]--;
        assert(pos > 0);
		g->node_idx[ g->begin[u] + pos -1] = v;  // set end node of this edge

        if (need_back) {
		    pos = in_degree[v]--;
		    g->r_node_idx[ g->r_begin[v] + pos -1] = u;
            assert(pos > 0);
        }
	}


	delete [] src;
	delete [] dest;
	delete [] degree;
	delete [] in_degree;

	return g;
}



/** 
 Create RMAT graph
	a, b, c : params
 */
gm_graph* create_RMAT_graph(index_t N, index_t M, 
        index_t rseed, bool need_back,
        double a, double b, double c, bool permute)
{
	double d;
	assert( a+ b + c < 1);
	d = 1 - (a + b + c);

   	gm_graph* g = new gm_graph();
   	g->allocate_memory(N, M, need_back);

	//----------------------------------------------
   	// generate edges
	//----------------------------------------------
	// 0. init
	index_t* src = new index_t[M];
	index_t* dest = new index_t[M];
	index_t* degree = new index_t[N];
	memset(degree, 0, sizeof(index_t)*N);
    index_t* in_degree = NULL;
    if (need_back) {
	    in_degree = new index_t[N];
	    memset(in_degree, 0, sizeof(index_t)*N);
    }

	index_t SCALE = (index_t) log2((double)N);
	
	// my favorite random seed
	srand48(rseed);

	// 1. edge-gen 
	for(index_t i = 0; i < M; i++)
	{
		index_t u = 1;
		index_t v = 1;
		index_t step = N/2;
		double av = a;
		double bv = b;
		double cv = c;
		double dv = d;

		double p = drand48();
		if (p < av) { // do nothing
		} else if (p < (av + bv)) {
			v += step;
		} else if (p < (av+bv+cv)) {
			u += step;
		} else {
			v += step;
			u += step;
		}
		for(index_t j=1; j < SCALE; j++) {
			step = step /2;
			double var = 0.1;
			av *= 0.95 + var * drand48();		// vary abcd by 10%
			bv *= 0.95 + var * drand48();
			cv *= 0.95 + var * drand48();
			dv *= 0.95 + var * drand48();

			double S = av + bv + cv + dv;
			av = av / S;
			bv = bv / S;
			cv = cv / S;
			dv = dv / S;

			// choose partition
			p = drand48();
			if (p < av) { // do nothing
			} else if (p < (av + bv)) {
				v += step;
			} else if (p < (av+bv+cv)) {
				u += step;
			} else {
				v += step;
				u += step;
			}
		}
	
		src[i] = u - 1;
		dest[i] = v - 1;

        // avoid self edges
        if (src[i] == dest[i]) {i=i-1; continue;}
	}	

	// 2. permutate vertice 
	// so that, one can't know what are the high-degree edges from node_id
	if (permute) {
		index_t* P = new index_t[N];
		for( index_t i = 0; i < N; i++)
			 P[i] = i;
		
		for( index_t i = 0; i < N; i++) {
			index_t j = (index_t) (N * drand48());
			index_t temp = P[j];
			P[j] = P[i]; 
			P[i] = temp;
		}

		for( index_t i = 0; i < M ; i++) {
			src[i] = P[src[i]];
			dest[i] = P[dest[i]];
		}

		delete [] P;
	}

	// 3. count degree 
	for( index_t i = 0; i < M ; i++) {
		degree[src[i]]++;
        if (need_back)
            in_degree[dest[i]]++;
    }

	// (It is possible this routine creates multi-edges between a node-pair)
	// (Should be check later, or be ignored now)


	// 4. Now setup G's data structures
	g->begin[0] = 0;
    if (need_back) g->r_begin[0] = 0;
	for(index_t i=1; i <=N; i++) {
		g->begin[i] = g->begin[i-1] + degree[i-1];
        if (need_back) {
		    g->r_begin[i] = g->r_begin[i-1] + in_degree[i-1];
        }
    }

	for(index_t i=0; i <M; i++) {
		index_t u = src[i];
		index_t v = dest[i];
		index_t pos = degree[u]--;

		g->node_idx[ g->begin[u] + pos -1] = v;  // set end node of this edge

        if (need_back) {
		    pos = in_degree[v]--;
		    g->r_node_idx[ g->r_begin[v] + pos -1] = u;
        }
	}


	delete [] src;
	delete [] dest ;
	delete [] degree;

    delete [] in_degree;

	return g;
}

#include <vector>
/*
gm_graph* create_graph_from_file(FILE* F)
{

   // format of txt file
   // file is sorted by from_node
   // #comment
   // from_node to_node

   index_t curr_num=0;
   index_t base = -1;

   std::vector<index_t> nodes;
   std::vector<index_t> edges;

   char line[1024];
   while (fgets(line,  1024, F) != NULL)
   {
        if (line[0] == '#')
            continue;

        index_t from, to;
        sscanf(line,"%d%d", &from, &to);
        
		if (base == -1) {
          	base = from;		//make first node as 0
		}

        {
            from = from - base;
            to = to - base;

            if (from < 0) continue;
            if (to < 0) continue;
        }

		// ignore non-sorted values
		if (from < curr_num) 
			continue;

        // start of new node
        if (from != curr_num) {
              nodes.push_back(edges.size());
              curr_num = from;
        }
        edges.push_back(to);
   }
   nodes.push_back(edges.size());

   index_t N = nodes.size() -1;
   index_t M = edges.size();

   // sanitize edges
   srand(16384);
   for(index_t i=0; i < M; i++)
		if (edges[i] >= N) edges[i] = rand() % N;

   gpgraph *g = new gpgraph();

   g->allocate_nodes(N);
   g->allocate_edges(M);

   memcpy(g->begin, &(nodes[0]), sizeof(index_t)*(N+1));
   memcpy(g->node_idx, &(edges[0]), sizeof(index_t)*M);

   return g;
}
*/


