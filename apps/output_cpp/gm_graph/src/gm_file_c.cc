#include "gm_file_handling.h"
#include "gm_file_c.h"

extern "C"
void* gmGenFileReaderOpen(const char* filename, int hdfs) {
  return new GM_Reader(filename, (bool)hdfs);
}

extern "C"
void gmGenFileReaderClose(void* reader) {
  ((GM_Reader*)reader)->terminate();
  delete (GM_Reader*)reader;
}

extern "C"
int gmGenFileReaderReadBytes(void* reader, char* buf, size_t num_bytes) {
  return ((GM_Reader*)reader)->readBytes(buf, num_bytes);
}

extern "C"
int gmGenFileReaderSeekCurrent(void* reader, long int pos) {
  return ((GM_Reader*)reader)->seekCurrent(pos);
}

extern "C"
int gmGenFileReaderIsDirectory(void* reader) { // 1 if reading all the files in the directory. 0 if reading single file
  return ((GM_Reader*)reader)->isDirectory();
}

extern "C"
void* gmGenFileWriterOpen(const char* filename, int hdfs) {
  return new GM_Writer(filename, (bool)hdfs);

}

extern "C"
void gmGenFileWriterClose(void* writer) {
  ((GM_Writer*)writer)->terminate();
  delete (GM_Writer*)writer;
}


extern "C"
void gmGenFileWriterWriteBytes(void* writer, char* buf, size_t num_bytes) {
  ((GM_Writer*)writer)->writeBytes(buf, num_bytes);
}

extern "C"
void gmGenFileWriterFlush(void* writer) {
  ((GM_Writer*)writer)->flush();
}

