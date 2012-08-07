#ifndef GM_H_
#define GM_H_
#include <math.h>
#include <stdlib.h>

#include "gm_graph_typedef.h"
#include "gm_graph.h"
#include "gm_runtime.h"
#include "gm_atomic_wrapper.h"
#include "gm_lock.h"
#include "gm_bitmap.h"
#include "gm_mem_helper.h"
#ifdef __HDFS__
  #include "gm_hdfs.h"
#endif

#include "gm_bfs_template.h"
#include "gm_dfs_template.h"
#include "gm_common_neighbor_iter.h"

#include "gm_set.h"
#include "gm_seq.h"
#include "gm_order.h"
#include "gm_collection.h"
#include "gm_map.h"

#endif
