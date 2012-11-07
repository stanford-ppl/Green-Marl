#ifndef GM_FILE_HANDLING_H_
#define GM_FILE_HANDLING_H_

#include <string>
#include <fstream>
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
class GM_LineReader {
  public:
    GM_LineReader (const char *filename, bool hdfs = false);

    void initialize();
    bool failed();
    void reset();
    bool getNextLine(std::string &line);
    void terminate();

  private:
    const char *filename_;
    bool hdfs_;
    bool failed_;
#ifdef HDFS
    JNIEnv *env_;
    jclass cls_;
    jobject lineReaderObj_;
    jmethodID getLineMethod_;
#else
    std::ifstream fs_;
#endif
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
    void flush ();

  private:
    const char *filename_;
    bool hdfs_;
    bool failed_;
#ifdef HDFS
    JNIEnv *env_;
    jclass cls_;
    jobject writerObj_;
    jmethodID writeMethod_;
    std::stringstream outstream_;
#else
    std::ofstream outstream_;
#endif
};

#endif /* GM_FILE_HANDLING_H_ */
