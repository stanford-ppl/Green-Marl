#ifndef GM_FILE_HANDLING_H_
#define GM_FILE_HANDLING_H_

#include <string>
#include <fstream>
#include <iostream>
#ifdef HDFS
#include <sstream>
#include <jni.h>
#endif

#ifdef HDFS
class GM_JNI_Handler {
  public:
    static GM_JNI_Handler* getInstance();
    bool failed();
    JNIEnv *env_;
    JavaVM *jvm_;
    void printAndClearException();

  private:
    GM_JNI_Handler();
    ~GM_JNI_Handler();

    static GM_JNI_Handler *singleton_;
    JavaVMOption  opts_[1];
    JavaVMInitArgs vmargs_;
    bool failed_;
};
#endif

/*
 * A class to read a file and return one line at a time.
 * This basically performs the function of std::getline using fprintf.
 * Additionally, this class has the options of reading from an NFS or a HDFS file system.
 */
class GM_Reader {
  public:
    GM_Reader (const char *filename, bool hdfs = false);

    void initialize();
    bool failed();
    void reset();
    bool getNextLine(std::string &line);
    int readBytes(char* buf, size_t num_bytes);
    int seekCurrent(long int pos);
    void terminate();
    int isDirectory() { return is_reading_directory_;}

  private:
    //const char *filename_;
    std::string filename_;
    bool hdfs_;
    bool failed_;
    int  is_reading_directory_; // 1- is direcoty, 0 - is not a direcotry

#ifdef HDFS
    JNIEnv *env_;
    jclass cls_;
    jobject readerObj_;
    jmethodID getLineMethod_;
    jmethodID getBytesMethod_;
    jmethodID seekCurrentMethod_;
    jmethodID isDirectoryMethod_;
#endif
    std::ifstream fs_;  // file system is enabled even when HDFS is enabled
};

class GM_Writer {
public:
    GM_Writer (const char *filename, bool hdfs = false);

    void initialize();
    bool failed();
    void terminate();

    void write (bool val);
    void write (int val);
    void write (long val);
    void write (float val);
    void write (double val);
    void write (const char *val);
    void write (std::string &val);
    void writeBytes(char* buf, size_t num_bytes);

    void flush ();

  private:
    //const char *filename_;
    std::string filename_;
    bool hdfs_;
    bool failed_;
#ifdef HDFS
    JNIEnv *env_;
    jclass cls_;
    jobject writerObj_;
    jmethodID writeMethod_;
    jmethodID writeBytesMethod_;
    jmethodID flushMethod_;
    std::stringstream outstream_;
#endif
    std::ofstream outstreamfs_;
};

#endif /* GM_FILE_HANDLING_H_ */
