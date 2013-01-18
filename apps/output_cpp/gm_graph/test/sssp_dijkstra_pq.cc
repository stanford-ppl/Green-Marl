#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include <limits.h>
#include <cmath>
#include <algorithm>
#include <limits>
#include <unordered_map>
#include <sparsehash/dense_hash_map>
//#include <queue>
#include <stack>
#include <omp.h>
#include "gm.h"
#include <ext/pb_ds/priority_queue.hpp>
#include <valgrind/callgrind.h>


using google::dense_hash_map;


class NodeHashTable {

public:

  NodeHashTable() : capacity(1024*32), size(0) {
    values = new void*[capacity];
    keys = new node_t[capacity];
    for (int i=0; i<capacity; i++) {
      values[i] = NULL;
    }
  }


  void* put(node_t key, void* val) {
    assert(val != NULL);
    int ind = (key < 0 ? -key : key) % capacity;
    //    printf("KEY %d ", key);
    if (values[ind] == NULL) {
      //      printf("IND %d\n", ind);
      //      fflush(stdout);
      keys[ind] = key;
      values[ind] = val;
      size++;
      return NULL;
    }
    else if (keys[ind] == key) {
      //      printf("SAME KEY 0\n");
      //      fflush(stdout);
      void* ret = values[ind];
      values[ind] = val;
      return ret;
    }
    else {
      //      printf("RESOLVING CONFLICT FOR %d\n", key);
      //     fflush(stdout);
      while (true) {
        int ind_org = ind;
        ind = (ind + 1) % capacity;
        while (true) {
          if ((values[ind] != NULL && keys[ind] == key) ||
              values[ind] == NULL ||
              ind == ind_org)
            break;
          ind = (ind + 1) % capacity;
        }
        if (values[ind] == NULL) {
          keys[ind] = key;
          values[ind] = val;
          size++;
          return NULL;
        }
        else if (keys[ind] == key) {
          //          printf("SAME KEY 1\n");
          //          fflush(stdout);
          void* ret = values[ind];
          values[ind] = val;
          return ret;
        }
        else {
          // resize
          //          printf("RESIZING\n");
          //          fflush(stdout);
          int old_capacity = capacity;
          void** old_values = values;
          node_t* old_keys = keys;
          capacity *= 2;
          values = new void*[capacity];
          keys = new node_t[capacity];
          for (int i=0; i<capacity; i++) {
            values[i] = NULL;
          }
          for (int i=0; i<old_capacity; i++) {
            if (old_values[i] != NULL) {
              put(old_keys[i], old_values[i]);
            }
          }
          delete old_values;
          delete old_keys;

          ind = (key < 0 ? -key : key) % capacity;
          if (values[ind] == NULL) {
            keys[ind] = key;
            values[ind] = val;
            size++;
            return NULL;
          }
          else if (keys[ind] == key) {
            void* ret = values[ind];
            values[ind] = val;
            return ret;
          }
          // else re-loop
        }
      }
    }    
  }


  void* remove(node_t key) {
    int ind = (key < 0 ? -key : key) % capacity;
    //    printf("REMOVE KEY %d IND %d\n", key, ind);
    //    fflush(stdout);

    if (values[ind] != NULL &&
        keys[ind] == key) {
      void* ret = values[ind];
      values[ind] = NULL;
      size--;
      return ret;
    }
    else {
      int ind_org = ind;
      ind = (ind + 1) % capacity;
      while (true) {
        if ((values[ind] != NULL && keys[ind] == key) ||
            ind == ind_org)
          break;
        ind = (ind + 1) % capacity;
      }
      if (values[ind] != NULL && keys[ind] == key) {
        void* ret = values[ind];
        values[ind] = NULL;
        size--;
        return ret;
      }
      else {
        return NULL;
      }
    }
  }

  void* get(node_t key) {
    int ind = (key < 0 ? -key : key) % capacity;
    if (values[ind] != NULL &&
        keys[ind] == key) {
      return values[ind];
    }
    else {
      int ind_org = ind;
      ind = (ind + 1) % capacity;
      while (true) {
        if ((values[ind] != NULL && keys[ind] == key) ||
            ind == ind_org)
          break;
        ind = (ind + 1) % capacity;
      }
      if (values[ind] != NULL && keys[ind] == key) {
        return values[ind];
      }
      else {
        return NULL;
      }
    }
  }


private:
  int capacity;
  void** values;
  node_t* keys;
  int size;

};
 
class MyVisitedNode {
public:
  node_t id;
  double cost;
  edge_t prevLink;
  MyVisitedNode* prevNode;
  
  MyVisitedNode(node_t n, double c, edge_t e, MyVisitedNode* p_n): id(n), cost(c), prevLink(e), prevNode(p_n) {}
  void reset(node_t n, double c, edge_t e, MyVisitedNode* p_n) {
    id = n;
    cost = c;
    prevLink = e;
    prevNode = p_n;
  }

  node_t getId() {
    return id;
  }

  double getCost() {
    return cost;
  }

  edge_t getPrevLink() {
    return prevLink;
  }

  MyVisitedNode* getPrevNode() {
    return prevNode;
  }
  
};


class MyVisitedNodeComparator : public std::binary_function<MyVisitedNode*, MyVisitedNode*, bool> {

public:
  bool operator()(const MyVisitedNode* lhs, const MyVisitedNode* rhs) const {  
    return lhs->cost > rhs->cost;  
  }  

};

/*
class NodeComparator {

public:
  bool operator()(const node_t lhs, const node_t rhs) const {  
    return lhs == rhs;  
  }  

};
*/


static edge_t DOUBLE_POSITIVE_INFINITY = numeric_limits<double>::max();
static edge_t INVALID_LINK_ID = numeric_limits<int>::max();
//static std::priority_queue<MyVisitedNode*, std::vector<MyVisitedNode*>, MyVisitedNodeComparator> pQueue;
static __gnu_pbds::priority_queue<MyVisitedNode*, MyVisitedNodeComparator, __gnu_pbds::binary_heap_tag> pQueue;
static std::unordered_map<node_t, MyVisitedNode*> pQueueMap(1024*32);
//static dense_hash_map<node_t, MyVisitedNode*, tr1::hash<node_t>, NodeComparator> pQueueMap(1024*32);
//static dense_hash_map<node_t, MyVisitedNode*> pQueueMap(1024*32);
//static NodeHashTable pQueueMap;


void pQueueErase(MyVisitedNode* n) {
  __gnu_pbds::priority_queue<MyVisitedNode*, MyVisitedNodeComparator, __gnu_pbds::binary_heap_tag>::const_iterator iter;
  for (iter = pQueue.begin(); iter != pQueue.end(); ++iter) {
    if (*iter == n)
      break;
  }
  if (iter != pQueue.end()) {
    pQueue.erase(iter);
  }

}

node_t getDstNode(gm_graph& G, edge_t e) {
  return G.node_idx[e];
}


edge_t* getOutLinks(gm_graph& G, node_t node_id, int& len) {
  len = G.begin[node_id+1] - G.begin[node_id];
  if (len == 0) return NULL;

  edge_t* outLinks = new edge_t[len];
  for (int i = 0, j = G.begin[node_id]; j < G.begin[node_id+1]; i++, j++) {
    outLinks[i] = j;
  }
  return outLinks;
}

/*
edge_t* getOutLinks(gm_graph& G, node_t node_id, int& len, edge_t* outLinks) {
  len = G.begin[node_id+1] - G.begin[node_id];
  if (len == 0) return NULL;

  for (int i = 0, j = G.begin[node_id]; j < G.begin[node_id+1]; i++, j++) {
    outLinks[i] = j;
  }
  return outLinks;
}
*/

void relax(gm_graph& G,
           MyVisitedNode* minElem, 
           edge_t nextLink,
           node_t currNode,
           node_t nextNode,
           node_t startNode,
           node_t endNode,
           std::unordered_map<node_t, MyVisitedNode*>& fullyExpandedNodes,
           double* edge_costs) {

  double linkCost = edge_costs[nextLink];
  double newCost = minElem->getCost() + linkCost;

  node_t nextNodeId = nextNode;
  
  if(fullyExpandedNodes.find(nextNodeId) != fullyExpandedNodes.end())
    return;
  
  bool isFirstTimeVisit = (pQueueMap.find(nextNodeId) == pQueueMap.end());
  MyVisitedNode* nextElem = (isFirstTimeVisit ? NULL : pQueueMap[nextNodeId]);
  //  MyVisitedNode* nextElem = (MyVisitedNode*)pQueueMap.get(nextNodeId);      
  //  bool isFirstTimeVisit = (nextElem == NULL);

  double oldCost = (isFirstTimeVisit)? DOUBLE_POSITIVE_INFINITY : nextElem->getCost();
  bool isCheaper = newCost < oldCost;
  
  if(!isFirstTimeVisit && !isCheaper)
    //The next element has been partially expanded and revisited (because 
    //it is in the priority queue),
    //now we are revisiting it again before it even gets a chanced to be
    //removed from the queue and expanded, therefore, we simply
    //do nothing since the new path is no better.
    return;
  
  MyVisitedNode* newNextElem = new MyVisitedNode(nextNode, newCost, nextLink, minElem);
  //  printf("OLD %f NEW %f %d\n", oldCost, newCost, nextNode);

  if (!isFirstTimeVisit) {
    //    printf("ERASE 0 %d\n", nextElem->getId());
    //    fflush(stdout);
    //    pQueueMap.remove(nextElem->getId());
    pQueueMap.erase(nextElem->getId());
    pQueueErase(nextElem); // same as in pure Java version (iterate)
    delete nextElem;
    
  }
  pQueue.push(newNextElem);
  //  printf("PUSH %d\n", newNextElem->getId());
  //  fflush(stdout);
  pQueueMap[nextNodeId] =  newNextElem;
  //  pQueueMap.put(nextNodeId, newNextElem);
}


void expand(gm_graph &G,
            MyVisitedNode* minElem,
            node_t startNode,
            node_t endNode, 
            std::unordered_map<node_t, MyVisitedNode*>& fullyExpandedNodes,
            double* edge_costs) {
  node_t minNodeId = minElem->getId(); 
  node_t currNode = minNodeId;

  edge_t currLink = minElem->getPrevLink();

  int outLinksLength;
  //  edge_t outLinks[64];
  edge_t* outLinks = getOutLinks(G, currNode, outLinksLength);

  if(outLinks != NULL) {

    for(int iLink = 0; iLink<outLinksLength; iLink++) {
      edge_t nextLink = outLinks[iLink];
      node_t nextNode = getDstNode(G, nextLink);
      relax(G, minElem, nextLink, currNode, nextNode, startNode, endNode, fullyExpandedNodes, edge_costs);
    }
    delete outLinks;
  }
  fullyExpandedNodes[minNodeId] = minElem;
}


MyVisitedNode* shortestPath(gm_graph& G, node_t startNode, node_t endNode, double* edge_costs) {
  std::unordered_map<node_t, MyVisitedNode*> fullyExpandedNodes(1024*32);
  //  fullyExpandedNodes.set_empty_key(-1);
  //  fullyExpandedNodes.set_deleted_key(-2);
  //  pQueueMap.set_empty_key(-1);
  //  pQueueMap.set_deleted_key(-2);
  
  MyVisitedNode* initialNode = new MyVisitedNode(startNode, 0.0, INVALID_LINK_ID, NULL);
  pQueue.push(initialNode);
  //  printf("PUSH %d\n", initialNode->getId());
  //  fflush(stdout);

  pQueueMap[initialNode->getId()] = initialNode;
  //  pQueueMap.put(initialNode->getId(), initialNode);

  MyVisitedNode* minElem;    
  MyVisitedNode* endPoint = NULL;
  while(true) {
    if (pQueue.size() == 0) {
      return NULL;  // no path found
    }
    minElem = (MyVisitedNode*)pQueue.top();
    pQueue.pop();
    //    printf("ERASE 1 %d\n", minElem->getId());
    //    fflush(stdout);

    int res = pQueueMap.erase(minElem->getId());
    assert(res == 1);
    //    MyVisitedNode* res = (MyVisitedNode*)pQueueMap.remove(minElem->getId());
    //    assert(res != NULL);

    if (minElem->getId() == endNode) {
      endPoint = minElem;
      break;
    }
    
    expand(G, minElem, startNode, endNode, fullyExpandedNodes, edge_costs);
  }

  return endPoint;
}




double computePath(MyVisitedNode* lastNode,
                   node_t startNodeId,
                   gm_graph& G,
                   std::stack<edge_t>& pathEdges,
                   std::stack<node_t>& pathNodes,
                   double* edge_costs) {
  MyVisitedNode* currNode = lastNode;
  double totalCost = 0.0;
  while (startNodeId != currNode->getId()) {
    edge_t prevLink = currNode->getPrevLink();
    pathEdges.push(prevLink);
    pathNodes.push(currNode->getId());
    totalCost += edge_costs[prevLink];
    currNode = currNode->getPrevNode();
  }
  return totalCost; 
}



  void printPath(node_t startNodeKey,
                 node_t endNodeKey,
                 std::stack<edge_t>& pathEdges,
                 std::stack<node_t>& pathNodes,
                 double totalCost,
                 gm_graph& G,
                 int cutoff,
                 long* network_edge_keys) {
    node_t startNodeId = G.nodekey_to_nodeid(startNodeKey);
    node_t endNodeId = G.nodekey_to_nodeid(endNodeKey);
    printf("%d -> %d\n", startNodeKey, endNodeKey);
    printf("    Costs are %lf\n", totalCost);
    printf("    Number of links is %d\n", pathEdges.size());
    node_t prev_n = startNodeId;
    while (true) {
      if (pathEdges.size() == 0 || --cutoff == 0) break;
      edge_t e = pathEdges.top();
      pathEdges.pop();
      node_t n = pathNodes.top();
      pathNodes.pop();
      assert(getDstNode(G, e) == n);
      printf("        %d: %d - %d\n", network_edge_keys[e], G.nodeid_to_nodekey(prev_n), G.nodeid_to_nodekey(n));
      prev_n = n;
    }
    
  }


int main(int argc, char** argv) {

    if (argc < 5) {
        printf ("Usage: ./sssp_path <input_file> <src_key> <dst_key> <dbg>\n");
        exit(1);
    }
    gm_rt_set_num_threads(1); // gm_runtime.h

    char *inputFile = argv[1];

    //------------------------------
    // Empty graph creation
    //------------------------------
    gm_graph G;

    std::vector<VALUE_TYPE> vprop_schema;
    // format for sample.adj
    vprop_schema.push_back(GMTYPE_DOUBLE);
    
    std::vector<VALUE_TYPE> eprop_schema;
    // format for sample.adj
    eprop_schema.push_back(GMTYPE_DOUBLE);
    eprop_schema.push_back(GMTYPE_LONG);


    std::vector<void*> vertex_props;
    std::vector<void*> edge_props;

    //------------------------------
    // Read adjacency list graph
    //------------------------------
    // File format:
    // 
    // src_node src_node_cost dst_node1_id edge1_cost edge1_key ... dst_nodeN_id edgeN_cost edge_N_id
    //
    //
    //    printf ("Loading graph from file \'%s\' in adjacency list format...\n", inputFile);

    //    struct timeval T3, T4;    
    //    gettimeofday(&T3, NULL);
    G.load_adjacency_list(inputFile, vprop_schema, eprop_schema, vertex_props, edge_props, " \t", false);
    //    gettimeofday(&T4, NULL);
    //    printf("MY DIJKSTRA CSR C - GRAPH LOADING TIME (ms): %lf\n", (T4.tv_sec - T3.tv_sec) * 1000 + (T4.tv_usec - T3.tv_usec) * 0.001);

    //------------------------------
    // Print graph details for manual verification 
    //------------------------------
    //    printf ("Number of nodes = %d\n", G.num_nodes());
    //    printf ("Number of edges = %d\n", G.num_edges());


    gm_rt_set_num_threads(1); // gm_runtime.h

    double* edge_costs = (double*)edge_props[0];
    long* network_edge_keys = (long*)edge_props[1];
    gm_node_seq Q;

    //    node_t startNodeKey = 199535084;
    //    node_t endNodeKey = 199926436;


    node_t startNodeKey = atol(argv[2]);
    node_t endNodeKey = atol(argv[3]);
    int dbg = atol(argv[4]);
     
    node_t startNodeId = G.nodekey_to_nodeid(startNodeKey);
    node_t endNodeId = G.nodekey_to_nodeid(endNodeKey);

    //    printf("GRAPH LOADED\n");
    //    fflush(stdout);
    struct timeval T1, T2;    
    gettimeofday(&T1, NULL);
    // compute all shortest paths from root
    //    CALLGRIND_START_INSTRUMENTATION;
    MyVisitedNode* lastNode = shortestPath(G, startNodeId, endNodeId, edge_costs);
    std::stack<edge_t> pathEdges;
    std::stack<node_t> pathNodes;
    // get specific instance from root to end
    double totalCost = computePath(lastNode, startNodeId, G, pathEdges, pathNodes, edge_costs);
    //   CALLGRIND_STOP_INSTRUMENTATION;
    gettimeofday(&T2, NULL);
    printf("MY DIJKSTRA CSR C - COMPUTATION RUNNING TIME (ms): %lf\n", (T2.tv_sec - T1.tv_sec) * 1000 + (T2.tv_usec - T1.tv_usec) * 0.001);
    //    printf("cost = %lf\n", totalCost);
        
    if (dbg != 0)
      printPath(startNodeKey, endNodeKey, pathEdges, pathNodes, totalCost, G, 21, network_edge_keys);


}
