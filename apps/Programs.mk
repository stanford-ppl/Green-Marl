ifndef PROGS
 ifeq ($(TARGET), gps)
   PROGS= pagerank avg_teen_cnt conduct hop_dist sssp random_bipartite_matching 
 else
   PROGS= test #pagerank conduct bc kosaraju adamicAdar v_cover sssp #random_bipartite_matching
 endif
endif
export PROGS
