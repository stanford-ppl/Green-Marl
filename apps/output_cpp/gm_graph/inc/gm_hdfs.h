#ifndef GM_GRAPH_HDFS_H
#define GM_GRAPH_HDFS_H
#include <stdio.h>
#include "gm_graph.h"

class gm_graph_hdfs : public gm_graph {
public:
	bool load_binary(char* filename);
};

#endif
