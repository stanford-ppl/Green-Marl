ifndef PROGS
 ifeq ($(TARGET), gps)
   PROGS= pagerank avg_nbr_age conduct random_bipartite_matching #sssp  
 else
   PROGS= pagerank conduct bc kosaraju adamicAdar v_cover sssp #random_bipartite_matching
 endif
endif
export PROGS

