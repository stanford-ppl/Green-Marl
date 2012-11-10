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

int gmGenFileReaderGetBytes(void* reader, char* buf, size_t num_bytes);

int gmGenFileReaderSeekCurrent(void* reader, long int pos);

#ifdef __cplusplus
}
#endif

#endif /* GM_FILE_HANDLING_H_ */


