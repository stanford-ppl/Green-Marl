#ifndef GM_GRAPH_HDFS_H
#define GM_GRAPH_HDFS_H
#include "gm_graph.h"

// libhdfs won't use the namenode from core-site.xml, hardcoding for now
//#define HDFS_NAMENODE "default"
//#define HDFS_PORT 0
#define HDFS_NAMENODE "namenode.ib.bunch"
#define HDFS_PORT 8020

class gm_graph_hdfs : public gm_graph {
public:
	bool load_binary(char* filename);
    #ifdef __AVRO__
	bool load_avro(char* filename);
    #endif
};

#endif
