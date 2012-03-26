ifndef PROGS
 ifeq ($(TARGET), gps)
   PROGS=pagerank avg_nbr_age conduct sssp 
 else
   PROGS=pagerank conduct bc kosaraju adamicAdar v_cover sssp 
 endif
endif
export PROGS

