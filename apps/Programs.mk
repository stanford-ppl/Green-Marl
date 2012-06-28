ifndef PROGS
 ifeq ($(TARGET), gps)
   PROGS= pagerank avg_teen_cnt conduct hop_dist sssp random_bipartite_matching  bc_random
 else
   PROGS= pagerank conduct bc kosaraju adamicAdar v_cover sssp #random_bipartite_matching
 endif
endif
export PROGS
