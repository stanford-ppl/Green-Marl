#ifndef GM_GRAPH_HDFS_H
#define GM_GRAPH_HDFS_H
#include <stdio.h>
#include "gm_graph.h"

#define HDFS_NAMENODE "namenode.ib.bunch"
#define HDFS_PORT 8020

class gm_graph_hdfs : public gm_graph {
public:
	bool load_binary(char* filename);
};

#endif
