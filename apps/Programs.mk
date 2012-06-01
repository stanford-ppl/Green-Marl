ifndef PROGS
 ifeq ($(TARGET), gps)
   PROGS= pagerank avg_teen_cnt conduct random_bipartite_matching sssp hop_dist
 else
   PROGS= shortestpath diameter randomwalk generator plotter communities pagerank conduct bc kosaraju adamicAdar v_cover sssp random_bipartite_matching
 endif
endif
export PROGS

