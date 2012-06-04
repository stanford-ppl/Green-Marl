#include "common_main.h"
#include "randomwalk.h"
#include "gm_set.h"
#include <map>

using namespace std;

class my_main: public main_t
{
public:
  gm_node_set* nodeSet;
  
  //--------------------------------------------------------
  // create 4 groups randomly
  //--------------------------------------------------------
  virtual bool prepare() {
    nodeSet = new gm_node_set(G.num_nodes());
    return true;
  }
  
  virtual bool run() {
    node_t seed = 8;
    randomwalk(G, seed, 0.5, *nodeSet);
    return true;
  }
  
  virtual bool post_process() {
    gm_node_set& ns = *nodeSet;
    gm_graph G_new;
    map<node_t, node_t> mapping;
    node_t current = 0;
    
    gm_node_set::seq_iter II = ns.prepare_seq_iteration();
    while(II.has_next()) {
      node_t node = II.get_next();
      G_new.add_node();
      mapping[node] = current;
      printf("%d -> %d\n", node, current);
      current++;
    }
    
    II = ns.prepare_seq_iteration();
    while(II.has_next()) {
      node_t node = II.get_next();
      for(edge_t edge = G.begin[node]; edge < G.begin[node + 1]; edge++) {
	node_t target = G.node_idx[edge];
	if(ns.is_in(target)) {
	  G_new.add_edge(mapping[node], mapping[target]);
	}
      }
    }
    
    G_new.store_binary("newgraph.bin");
    
    return true;
  }
};

int main(int argc, char** argv)
{
  my_main M;
  M.main(argc, argv);
}
