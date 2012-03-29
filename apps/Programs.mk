ifndef PROGS
 ifeq ($(TARGET), gps)
   PROGS=pagerank avg_nbr_age age_diff conduct #sssp  
 else
   PROGS=pagerank conduct bc kosaraju adamicAdar v_cover sssp #random_bipartite_matching
 endif
endif
export PROGS

