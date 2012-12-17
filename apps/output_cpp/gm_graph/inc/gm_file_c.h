#ifndef GM_FILE_HANDLING_H_
#define GM_FILE_HANDLING_H_

/*
 * Interface with C code for handling operations on regular files and
 * HDFS.
 */
#ifdef __cplusplus
extern "C" {
#endif

void* gmGenFileReaderOpen(const char* filename, int hdfs);

void gmGenFileReaderClose(void* reader);

int gmGenFileReaderReadBytes(void* reader, char* buf, size_t num_bytes);

int gmGenFileReaderSeekCurrent(void* reader, long int pos);

int gmGenFileReaderIsDirectory(void* reader); // 1 if reading all the files in the directory. 0 if reading single file

void* gmGenFileWriterOpen(const char* filename, int hdfs);

void gmGenFileWriterClose(void* writer);

void gmGenFileWriterWriteBytes(void* writer, char* buf, size_t num_bytes);

void gmGenFileWriterFlush(void* writer);

#ifdef __cplusplus
}
#endif

#endif /* GM_FILE_HANDLING_H_ */


