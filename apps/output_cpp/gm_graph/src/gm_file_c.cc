#include "gm_file_handling.h"
#include "gm_file_c.h"

extern "C"
void* gmGenFileReaderOpen(const char* filename, int hdfs) {
  return new GM_Reader(filename, (bool)hdfs);
}

extern "C"
void gmGenFileReaderClose(void* reader) {
  delete (GM_Reader*)reader;
}

extern "C"
int gmGenFileReaderGetBytes(void* reader, char* buf, size_t num_bytes) {
  return ((GM_Reader*)reader)->getBytes(buf, num_bytes);
}

extern "C"
int gmGenFileReaderSeekCurrent(void* reader, long int pos) {
  return ((GM_Reader*)reader)->seekCurrent(pos);
}

