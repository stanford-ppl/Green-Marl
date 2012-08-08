#include <hdfs.h>
#include "gm_hdfs.h"

bool gm_graph_hdfs::load_binary(char* filename) {
    clear_graph();
    int32_t key;
    int i;
    hdfsFS fs;
    hdfsFile f;

	fs = hdfsConnect(HDFS_NAMENODE, HDFS_PORT);
	if(fs == NULL) {
		fprintf(stderr, "Failed to connect to hdfs.\n");
		goto error_return_noclose;
	}

	i = hdfsExists(fs, filename);
	if (i != 0) {
		fprintf(stderr, "Failed to validate existence of %s\n", filename);
		goto error_return_noclose;
	}

	f = hdfsOpenFile(fs, filename, O_RDONLY, 0, 0, 0);
	if (f == NULL) {
		fprintf(stderr, "Failed to open %s for reading.\n", filename);
		goto error_return_noclose;
	}

    // write it 4B wise?
	i = hdfsRead(fs, f, &key, 4); //TODO should this be 4 or 1?
    if ((i != 4) || (key != MAGIC_WORD)) {
        fprintf(stderr, "wrong file format, KEY mismatch: %d, %x\n", i, key);
        goto error_return;
    }

	i = hdfsRead(fs, f, &key, 4); // index size (4B)
    if (key != sizeof(node_t)) {
        fprintf(stderr, "node_t size mismatch:%d (expect %ld)\n", key, sizeof(node_t));
        goto error_return;
    }

	i = hdfsRead(fs, f, &key, 4); // index size (4B)
    if (key != sizeof(edge_t)) {
        fprintf(stderr, "edge_t size mismatch:%d (expect %ld)\n", key, sizeof(edge_t));
        goto error_return;
    }

    //---------------------------------------------
    // need back, numNodes, numEdges
    //---------------------------------------------
    node_t N;
    edge_t M;
	i = hdfsRead(fs, f, &N, sizeof(node_t));
    if (i != sizeof(node_t)) {
        fprintf(stderr, "Error reading numNodes from file \n");
        goto error_return;
    }
	i = hdfsRead(fs, f, &M, sizeof(edge_t));
    if (i != sizeof(edge_t)) {
        fprintf(stderr, "Error reading numEdges from file \n");
        goto error_return;
    }

    allocate_memory_for_frozen_graph(N, M);

    for (node_t i = 0; i < N + 1; i++) {
        edge_t key;
        int k = hdfsRead(fs, f, &key, sizeof(edge_t));
        if ((k != sizeof(edge_t))) {
            fprintf(stderr, "Error reading node begin array\n");
            goto error_return;
        }
        this->begin[i] = key;
    }

    for (edge_t i = 0; i < M; i++) {
        node_t key;
    	int k = hdfsRead(fs, f, &key, sizeof(node_t));
        if ((k != sizeof(node_t))) {
            fprintf(stderr, "Error reading edge-end array\n");
            goto error_return;
        }
        this->node_idx[i] = key;
    }

    hdfsCloseFile(fs, f);
    _frozen = true;
    return true;

    error_return: hdfsCloseFile(fs, f);
    error_return_noclose: clear_graph();
    return false;
}
