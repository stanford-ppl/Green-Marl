#ifndef GM_INTERNAL_H
#define GM_INTERNAL_H

//-------------------------------------------------------------------------------
// This is the internal 'shared' header for the modules in the library.
// This header includes all the 'leaf' headers, which are common to each module
//-------------------------------------------------------------------------------
// If the following flag is on, we let the correct thread 'touches' the data strcutre
// for the first time, so that the memory is allocated in the corresponding socket.
#define GM_GRAPH_NUMA_OPT   1   

// defines the size of edge_t and node_t
#include "gm_graph_typedef.h"  

// include the correct platform dependency
#if defined(__x86_64__) || defined(__i386__)
 #include "../platform/x86/inc/gm_platform_helpers.h"
#else
 #if defined(__sparc)
  #if defined (__ORACLE__)
   #include "../platform/sparc/inc/gm_platform_helpers.h"
  #else
   #error "We need __ORACLE__ Sparc environment" 
  #endif
 #else
   #error "We need x86 (32bit or 64bit) or Sparc environment" 
 #endif
#endif


#endif
