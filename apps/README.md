Example Algorithms
====================================


1. adamicAdar
-------------------------------------
Description:

C++: Yes

GPS/Giraph: No - at the moment Green-Marl does not support multiple procedure definitions in one file when using the GPS or Giraph back-end.


2. avg_teen_count
-------------------------------------
Description: Computes the average number of nodes that are followed by nodes that are 'younger' than 20 and are themselves 'older' than K.

C++: Yes

GPS/Giraph: Yes


3. b2
-------------------------------------
Description: A test for the DFS implementation

C++: Yes

GPS/Giraph: Yes


4. bc
-------------------------------------
Description: Computes the betweenness centrality value for the graph.

C++: Yes

GPS/Giraph: No - the Node_Sequence collection-type is not yet supported for the GPS or Giraph back-end yet.


5. bc_random
-------------------------------------
Description: Computes an estimation for the betweenness centrality value for the graph.

C++: Yes

GPS/Giraph: Yes


6. communities
-------------------------------------
Description: Computes the different communities in a graph.

C++: Yes

GPS/Giraph: No - Maps are not supported for the GPS or Giraph back-end yet.


7. conduct
-------------------------------------
Description: Computes how strongly a group of nodes is connected with the rest of the graph.

C++: Yes

GPS/Giraph: Yes


8. hop-dist
-------------------------------------
Description: Computes the distance of every node from a root node.

C++: Yes

GPS/Giraph: Yes


9. kosaraju
-------------------------------------
Description: Finds strongly connected components using Kosaraju's Algorithm.

C++: Yes

GPS/Giraph: No - the Node_Sequence collection-type is not yet supported for the GPS or Giraph back-end yet.


10. pagerank
-------------------------------------
Description: Computes the pagerank value for every node in the graph.

C++: Yes

GPS/Giraph: Yes


11. potential-friends
-------------------------------------
Description: Computes a set of potential friends for every node using triangle closing.

C++: Yes

GPS/Giraph: No - Giraph does not support accessing a node's neighbor's neighbors.
Additionally, node-properties of collections are not supported for the GPS or Giraph back-end yet.


12. random_bipartite_matching
-------------------------------------
Description: Computes a random matching for a bipartite graph.

C++: Yes

GPS/Giraph: Yes


13. random_node_sampling
-------------------------------------
Description: Samples the graph by picking nodes randomly.

C++: Yes

GPS/Giraph: No - the Node_Set collection-type is not yet supported for the GPS or Giraph back-end yet.


14. random_degree_node_sampling
-------------------------------------
Description: Samples the graph by picking nodes randomly with the probability being proportional to the node's degree.

C++: Yes

GPS/Giraph: No - the Node_Set collection-type is not yet supported for the GPS or Giraph back-end yet.


15. random_walk_sampling_with_random_jump
-------------------------------------
Description: Samples the graph by performing a random-walk with random jumps back to the starting node.

C++: Yes

GPS/Giraph: No - the Node_Sequence collection-type is not yet supported for the GPS or Giraph back-end yet.


16. sssp
-------------------------------------
Description: Computes the distance of every node from one destination node according to the shortest path.

C++: Yes

GPS/Giraph: Yes


17. sssp_path
-------------------------------------
Description: Computes the shortest paths from one destination node to every other node in the graph and returns the shortest path to a specific node.

C++: Yes

GPS/Giraph: No - at the moment Green-Marl does not support multiple procedure definitions in one file when using the GPS or Giraph back-end.


18. triangle_counting
-------------------------------------
Description: Computes the number of closed triangles in the graph.

C++: Yes

GPS/Giraph: No - Giraph does not support accessing a node's neighbor's neighbors.


19. v_cover
-------------------------------------
Description: Computes a vertex cover of a graph and returns the number of vertices in the cover.

C++: Yes

GPS/Giraph: No - the algorithm uses function that are not yet implemented for the GPS or Giraph back-end.