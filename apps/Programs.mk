ifndef PROGS
 ifeq ($(TARGET), gps)
   PROGS= pagerank avg_teen_cnt conduct hop_dist sssp random_bipartite_matching bc_random
 else
  ifeq ($(TARGET), giraph)
    PROGS= pagerank avg_teen_cnt conduct hop_dist sssp random_bipartite_matching bc_random
  else
    PROGS= communities bc_random randomnodesampling triangle_counting potential_friends pagerank avg_teen_cnt conduct bc kosaraju adamicAdar v_cover sssp #random_bipartite_matching sssp_path
  endif
 endif
endif
export PROGS
