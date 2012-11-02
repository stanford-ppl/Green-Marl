#include "gm_file_handling.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef HDFS
#include <hdfs.h>
#endif

GM_LineReader::GM_LineReader (const char *filename, bool hdfs) {
    filename_ = filename;
    hdfs_ = hdfs;
    initialize();
}

void GM_LineReader::initialize() {
#ifdef HDFS
    // Initialize for reading a file from HDFS
#else
    // Initialize for reading a file from NFS
    fs_.open(filename_);
    failed_ = false;
    if (fs_.fail()) {
        fprintf (stderr, "Cannot open %s for reading\n", filename_);
        failed_ = true;
    }
#endif  // HDFS
}

bool GM_LineReader::failed() {
    return failed_;
}

void GM_LineReader::reset() {
#ifdef HDFS
#else
    fs_.clear();
    fs_.seekg(0, std::ios::beg);
    if (fs_.fail()) {
        fprintf (stderr, "Error moving file pointer to the beginning of file %s\n", filename_);
    }
#endif  // HDFS
}

bool GM_LineReader::getNextLine(std::string &line) {
#ifdef HDFS
#else
    std::getline(fs_, line);
    return ! fs_.fail();
#endif  // HDFS
}

void GM_LineReader::terminate() {
    fs_.close();
}
