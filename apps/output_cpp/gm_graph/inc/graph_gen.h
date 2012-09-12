/*
 * graph_gen.h
 *
 *  Created on: Sep 11, 2012
 *      Author: raghav
 */
#ifndef GRAPH_GEN_H_
#define GRAPH_GEN_H_

#include "gm_graph.h"

gm_graph* create_uniform_random_graph(node_t N, edge_t M, long seed, bool use_xorshift_rng);
gm_graph* create_uniform_random_graph2(node_t N, edge_t M, long seed);
gm_graph* create_uniform_random_nonmulti_graph(node_t N, edge_t M, long seed);

#endif /* GRAPH_GEN_H_ */
