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
    opts_[0].optionString = (char *)"-Djava.class.path=/cm/shared/apps/hadoop/current/hadoop-core-0.20.2-cdh3u4.jar:/cm/shared/apps/hadoop/current/lib/commons-logging-1.0.4.jar:/cm/shared/apps/hadoop/current/lib/guava-r09-jarjar.jar:../javabin/";
    memset(&vmargs_, 0, sizeof(vmargs_));
    vmargs_.version = JNI_VERSION_1_6;
    vmargs_.nOptions = 1;
    vmargs_.options = opts_;

    // Create a JVM
    long status = JNI_CreateJavaVM(&jvm_, (void **)&env_, &vmargs_);
    if (status == JNI_ERR) {
        fprintf (stderr, "JNI Error: Cannot create JVM\n");
        failed_ = true;
        return;
    }

    // Find the HDFS Line Reader clas
    cls_ = env_->FindClass("HDFSLineReader");
    if (cls_ == 0) {
        fprintf (stderr, "JNI Error: Cannot find class\n");
        failed_ = true;
        return;
    }


    // Get the methodID of the constructor of LineReader class that takes java.lang.String as the only input parameter
    jmethodID lineReaderConstructor = env_->GetMethodID(cls_, "<init>", "(Ljava/lang/String;)V");
    if (lineReaderConstructor == 0) {
        fprintf (stderr, "JNI Error: Cannot get Constructor\n");
        failed_ = true;
        return;
    }

    // Create a new object of LineReader class, and also invoke its constructor
    lineReaderObj_ = env_->NewObject(cls_, lineReaderConstructor, env_->NewStringUTF(filename_));
    if (lineReaderObj_ == 0) {
        fprintf (stderr, "JNI Error: Cannot create new object of LineReader class\n");
        failed_ = true;
        return;
    }

    // Get the methodID of getLine in LineReader class
    getLineMethod_ = env_->GetMethodID(cls_, "getLine", "()Ljava/lang/String;");
    if (getLineMethod_ == 0) {
        fprintf (stderr, "JNI Error: Cannot get getLine method in LineReader\n");
        failed_ = true;
        return;
    }
    failed_ = false;
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
    // Get the methodID of reset in LineReader class
    jmethodID resetMethod = env_->GetMethodID(cls_, "reset", "()V");
    if (resetMethod == 0) {
        fprintf (stderr, "JNI Error: Cannot get reset method in LineReader\n");
        failed_ = true;
        return;
    }

    // Call the reset method in LineReader class
    env_->CallVoidMethod(lineReaderObj_, resetMethod);
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
    // Call getLine method in LineReader class
    jobject strObj = env_->CallObjectMethod(lineReaderObj_, getLineMethod_, "");
    if (strObj == 0) {
        return false; // indicating end of file
    }

    // Convert the return object into C string
    const char* str = env_->GetStringUTFChars((jstring) strObj, NULL);
    if (str == 0) {
        fprintf (stderr, "JNI Error: Cannot convert java.lang.String to C String\n");
        return false;
    }
    line = str;
    env_->ReleaseStringUTFChars((jstring) strObj, str);
    return true;
#else
    std::getline(fs_, line);
    return ! fs_.fail();
#endif  // HDFS
}

void GM_LineReader::terminate() {
#ifdef HDFS
    // Get the methodID of terminate in LineReader class
    jmethodID terminateMethod = env_->GetMethodID(cls_, "terminate", "()V");
    if (terminateMethod == 0) {
        fprintf (stderr, "JNI Error: Cannot get terminate method in LineReader\n");
        failed_ = true;
        return;
    }

    // Call the terminate method in LineReader class
    env_->CallVoidMethod(lineReaderObj_, terminateMethod);

    // Destroy the jvm
    jvm_->DestroyJavaVM();
#else
    fs_.close();
#endif
}
