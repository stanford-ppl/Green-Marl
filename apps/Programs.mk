ifndef PROGS
 ifeq ($(TARGET), gps)
   PROGS= pagerank avg_teen_cnt conduct hop_dist sssp random_bipartite_matching bc_random
 else
  ifeq ($(TARGET), giraph)
    PROGS= pagerank avg_teen_cnt conduct hop_dist sssp random_bipartite_matching bc_random triangle_counting_directed parallel_random_walk_jump_sampling
  else
    PROGS= sssp_path hop_dist communities bc_random triangle_counting potential_friends pagerank avg_teen_cnt conduct bc kosaraju adamicAdar v_cover sssp random_walk_sampling_with_random_jump random_degree_node_sampling random_node_sampling sssp_path_adj bc_adj sssp_dijkstra bidir_dijkstra
  endif
 endif
endif
export PROGS
