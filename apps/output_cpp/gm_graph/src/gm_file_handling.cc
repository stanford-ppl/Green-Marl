#include "gm_file_handling.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <map>
#ifdef HDFS
#include <hdfs.h>
#endif

/***********************************************************
 * Definitions for GM_JNI_Handler class
 **********************************************************/
#ifdef HDFS
GM_JNI_Handler* GM_JNI_Handler::singleton_ = NULL;

GM_JNI_Handler* GM_JNI_Handler::getInstance() {
    if (singleton_ == NULL) {
        singleton_ = new GM_JNI_Handler();
    }
    return singleton_;
}

bool GM_JNI_Handler::failed() {
    return failed_;
}

extern bool gm_read_setup_file(std::map<std::string, std::string>& setup, bool export_env);

GM_JNI_Handler::GM_JNI_Handler() {

    // read setup file and set them into environment variable
    // (already exisitng env variables will stay untouched)
    std::map<std::string, std::string> S;
    gm_read_setup_file(S, true);

    char buffer[1024*64];
    const char* GMTop         = getenv("GM_TOP") == NULL ? "" : getenv("GM_TOP");
    const char* HadoopHome    = getenv("HADOOP_HOME") == NULL ? "" : getenv("HADOOP_HOME");
    const char* HadoopCoreJar = getenv("HADOOP_CORE_JAR") == NULL ? "" : getenv("HADOOP_CORE_JAR");
    const char* LoggingJar = "commons-logging-1.0.4.jar";
    const char* GuavaJar = "guava-r09-jarjar.jar";

    sprintf(buffer, "-Djava.class.path=%s/%s:%s/lib/%s:%s/lib/%s:%s/apps/output_cpp/gm_graph/javabin/ -XX:+UseOSErrorReporting", 
            HadoopHome, HadoopCoreJar,
            HadoopHome, LoggingJar,
            HadoopHome, GuavaJar, 
            GMTop);
    printf("buffer = %s\n", buffer);
    // Initialize parameters for creating a JavaVM
    //opts_[0].optionString = (char *)"-Djava.class.path=/cm/shared/apps/hadoop/current/hadoop-core-0.20.2-cdh3u4.jar:/cm/shared/apps/hadoop/current/lib/commons-logging-1.0.4.jar:/cm/shared/apps/hadoop/current/lib/guava-r09-jarjar.jar:../javabin/";
    opts_[0].optionString = buffer;
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

    failed_ = false;
}

GM_JNI_Handler::~GM_JNI_Handler() {
    if (singleton_ != NULL && (! failed_)) {
        // Destroy the jvm
        jvm_->DestroyJavaVM();
        singleton_ = NULL;
    }
}

#endif

/***********************************************************
 * Method definitions for GM_Reader class
 **********************************************************/

GM_Reader::GM_Reader (const char *filename, bool hdfs) {
    filename_ = filename;
    hdfs_ = hdfs;
    initialize();
#ifndef HDFS
    assert (hdfs == false);
#endif
}

void GM_Reader::initialize() {
    if (hdfs_) {
#ifdef HDFS
        // Initialize for reading a file from HDFS
        // Get the JNI environment from the GM_JNI_Handler
        GM_JNI_Handler *jni_handler = GM_JNI_Handler::getInstance();
        if (jni_handler->failed()) {
            fprintf (stderr, "JNI Error: Initialization failed\n");
            failed_ = true;
            return;
        }
        env_ = jni_handler->env_;

        // Find the HDFSReader class
        cls_ = env_->FindClass("HDFSReader");
        if (cls_ == 0) {
            fprintf (stderr, "JNI Error: Cannot find class HDFSReader\n");
            failed_ = true;
            return;
        }

        // Get the methodID of the constructor of HDFSReader class that takes java.lang.String as the only input parameter
        jmethodID readerConstructor = env_->GetMethodID(cls_, "<init>", "(Ljava/lang/String;)V");
        if (readerConstructor == 0) {
            fprintf (stderr, "JNI Error: Cannot get the constructor of HDFSReader class\n");
            failed_ = true;
            return;
        }

        // Create a new object of HDFSReader class, and also invoke its constructor
        readerObj_ = env_->NewObject(cls_, readerConstructor, env_->NewStringUTF(filename_.c_str()));
        if (readerObj_ == 0) {
            fprintf (stderr, "JNI Error: Cannot create new object of HDFSReader class\n");
            failed_ = true;
            return;
        }

        // Get the methodID of getLine in HDFSReader class
        getLineMethod_ = env_->GetMethodID(cls_, "getLine", "()Ljava/lang/String;");
        if (getLineMethod_ == 0) {
            fprintf (stderr, "JNI Error: Cannot get getLine method in HDFSReader\n");
            failed_ = true;
            return;
        }

        // Get the methodID of getBytes in HDFSReader class
        getBytesMethod_ = env_->GetMethodID(cls_, "getBytes", "(I)[B");
        if (getBytesMethod_ == 0) {
            fprintf (stderr, "JNI Error: Cannot get getBytes method in HDFSReader\n");
            failed_ = true;
            return;
        }

        // Get the methodID of seekCurrent in HDFSReader class
        seekCurrentMethod_ = env_->GetMethodID(cls_, "seekCurrent", "(J)I");
        if (seekCurrentMethod_ == 0) {
            fprintf (stderr, "JNI Error: Cannot get seekCurrent method in HDFSReader\n");
            failed_ = true;
            return;
        }
        failed_ = false;
#else
        failed_ = true;
        return;
#endif  // HDFS
    }
    //#else
    else {
        // Initialize for reading a file from NFS
        fs_.open(filename_.c_str());
        failed_ = false;
        if (fs_.fail()) {
            fprintf (stderr, "Cannot open %s for reading\n", filename_.c_str());
            failed_ = true;
        }
    }
    //#endif  // HDFS
}

bool GM_Reader::failed() {
    return failed_;
}

void GM_Reader::reset() {
    if (hdfs_)
    {
#ifdef HDFS
        // Get the methodID of reset in HDFSReader class
        jmethodID resetMethod = env_->GetMethodID(cls_, "reset", "()V");
        if (resetMethod == 0) {
            fprintf (stderr, "JNI Error: Cannot get reset method in HDFSReader\n");
            failed_ = true;
            return;
        }

        // Call the reset method in HDFSReader class
        env_->CallVoidMethod(readerObj_, resetMethod);
#endif
    }
    else {
        fs_.clear();
        fs_.seekg(0, std::ios::beg);
        if (fs_.fail()) {
            fprintf (stderr, "Error moving file pointer to the beginning of file %s\n", filename_.c_str());
        }
    }
}

bool GM_Reader::getNextLine(std::string &line) {
    if (hdfs_)
    {
#ifdef HDFS
        // Call getLine method in HDFSReader class
        jobject strObj = env_->CallObjectMethod(readerObj_, getLineMethod_, "");
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
        return false;
#endif // HDFS
    } else {
        std::getline(fs_, line);
        return ! fs_.fail();
    }
}

int GM_Reader::readBytes(char* buf, size_t num_bytes) {
    if (hdfs_) {
#ifdef HDFS
        // Call getBytes method in HDFSReader class
        jobject byteArray = env_->CallObjectMethod(readerObj_, getBytesMethod_, (jint)num_bytes);
        if (byteArray == 0) {
            return 0;
        }
        jbyte* bytes = env_->GetByteArrayElements((jbyteArray) byteArray, NULL);
        if (bytes == NULL) {
            return 0;
        }
        int bytes_read = env_->GetArrayLength((jarray)byteArray);
        memcpy(buf, bytes, bytes_read);
        env_->ReleaseByteArrayElements((jbyteArray) byteArray, bytes, JNI_ABORT); // no changes to copy back
        return bytes_read;
#else
        return 0;
#endif
    } else {
        fs_.read(buf, num_bytes);
        return fs_.gcount();
    }
}

int GM_Reader::seekCurrent(long int pos) {
    if (hdfs_) {
#ifdef HDFS
        // Call seekCurrent method in HDFSReader class
        return env_->CallIntMethod(readerObj_, seekCurrentMethod_, (jlong)pos);
#else
        return 0;
#endif
    } else {
        fs_.seekg(pos, std::ios::cur);    
        return 0; // TODO: error check?
    }
}


void GM_Reader::terminate() {
    if (hdfs_) {
#ifdef HDFS
        // Get the methodID of terminate in HDFSReader class
        jmethodID terminateMethod = env_->GetMethodID(cls_, "terminate", "()V");
        if (terminateMethod == 0) {
            fprintf (stderr, "JNI Error: Cannot get terminate method in HDFSReader\n");
            failed_ = true;
            return;
        }

        // Call the terminate method in HDFSReader class
        env_->CallVoidMethod(readerObj_, terminateMethod);
#else
        return ;
#endif
    } else {
        fs_.close();
    }
}

/***********************************************************
 * Method definitions for GM_Writer class
 **********************************************************/

GM_Writer::GM_Writer (const char *filename, bool hdfs) {
    filename_ = filename;
    hdfs_ = hdfs;
    initialize();
#ifndef HDFS
    assert(hdfs_ == false);
#endif
}

void GM_Writer::initialize () {
    if (hdfs_) {
#ifdef HDFS
        // Initialize for writing a file to HDFS
        // Get the JNI environment from the GM_JNI_Handler
        GM_JNI_Handler *jni_handler = GM_JNI_Handler::getInstance();
        if (jni_handler->failed()) {
            fprintf (stderr, "JNI Error: Initialization failed\n");
            failed_ = true;
            return;
        }
        env_ = jni_handler->env_;

        // Find the HDFSWriter class
        cls_ = env_->FindClass("HDFSWriter");
        if (cls_ == 0) {
            fprintf (stderr, "JNI Error: Cannot find class HDFSWriter\n");
            failed_ = true;
            return;
        }


        // Get the methodID of the constructor of HDFSWriter class that takes java.lang.String as the only input parameter
        jmethodID writerConstructor = env_->GetMethodID(cls_, "<init>", "(Ljava/lang/String;)V");
        if (writerConstructor == 0) {
            fprintf (stderr, "JNI Error: Cannot get Constructor of HDFSWriter class with String as the only input parameter\n");
            failed_ = true;
            return;
        }

        // Create a new object of HDFSWriter class, and also invoke its constructor
        writerObj_ = env_->NewObject(cls_, writerConstructor, env_->NewStringUTF(filename_.c_str()));
        if (writerObj_ == 0) {
            fprintf (stderr, "JNI Error: Cannot create new object of HDFSWriter class\n");
            failed_ = true;
            return;
        }

        // Get the methodID of getLine in HDFSWriter class
        writeMethod_ = env_->GetMethodID(cls_, "write", "(Ljava/lang/String;)V");
        if (writeMethod_ == 0) {
            fprintf (stderr, "JNI Error: Cannot get write method in HDFSWriter\n");
            failed_ = true;
            return;
        }

        // Get the methodID of writeBytes in HDFSWriter class
        writeBytesMethod_ = env_->GetMethodID(cls_, "writeBytes", "([B)V");
        if (writeBytesMethod_ == 0) {
            fprintf (stderr, "JNI Error: Cannot get writeBytes method in HDFSWriter\n");
            failed_ = true;
            return;
        }

        // Get the methodID of flush in HDFSWriter class
        flushMethod_ = env_->GetMethodID(cls_, "flush", "()V");
        if (flushMethod_ == 0) {
            fprintf (stderr, "JNI Error: Cannot get flush method in HDFSWriter\n");
            failed_ = true;
            return;
        }
        failed_ = false;
#else
        failed_ = true;
#endif // HDFS
    } else {
        // Initialize for writing a file to NFS
        outstreamfs_.open(filename_.c_str());
        failed_ = false;
        if (outstreamfs_.fail()) {
            fprintf (stderr, "Cannot open %s for writing\n", filename_.c_str());
            failed_ = true;
        }
    }
}

bool GM_Writer::failed() {
    return failed_;
}

void GM_Writer::terminate() {
    if (hdfs_) {
#ifdef HDFS
        flush();
        // Get the methodID of terminate in HDFSWriter class
        jmethodID terminateMethod = env_->GetMethodID(cls_, "terminate", "()V");
        if (terminateMethod == 0) {
            fprintf (stderr, "JNI Error: Cannot get terminate method in HDFSWriter\n");
            failed_ = true;
            return;
        }
        // Call the terminate method in HDFSWriter class
        env_->CallVoidMethod(writerObj_, terminateMethod);
#else 
        return;
#endif
    } else {
        outstreamfs_.close();
    }
}

#ifdef HDFS 
#define WRITE_STREAM(val) if (hdfs_) {outstream_ << val;} else { outstreamfs_ << val;}
#else
#define WRITE_STREAM(val) if (hdfs_) {} else { outstreamfs_ << val;}
#endif

void GM_Writer::write (bool val) {
    if (hdfs_) {
#ifdef HDFS
        outstream_ << std::boolalpha << val;
#endif
    } else {
        outstreamfs_ << std::boolalpha << val;
    }
}

void GM_Writer::write (int val) {
    WRITE_STREAM(val);
}

void GM_Writer::write (long val) {
    WRITE_STREAM(val);
}

void GM_Writer::write (float val) {
    WRITE_STREAM(val);
}

void GM_Writer::write (double val) {
    WRITE_STREAM(val);
}

void GM_Writer::write (const char *val) {
    WRITE_STREAM(val);
}

void GM_Writer::write (std::string &val) {
    WRITE_STREAM(val);
}

void GM_Writer::writeBytes(char* buf, size_t num_bytes) {
    if (hdfs_) {
#ifdef HDFS
        jobject byteArray = env_->NewByteArray((jsize)num_bytes);
        env_->SetByteArrayRegion((jbyteArray)byteArray, 0, num_bytes, (jbyte*)buf);
        env_->CallVoidMethod(writerObj_, writeBytesMethod_, byteArray);
#else
        return;
#endif
    }
    else {
        outstreamfs_.write(buf, num_bytes);
    }
}


void GM_Writer::flush () {
    if (hdfs_) {
#ifdef HDFS
        if (outstream_.str() == "") {
            env_->CallVoidMethod(writerObj_, flushMethod_, "");
        }
        else {
            // write the stringstream to file through a JNI call
            // Call write method in HDFSWriter class
            env_->CallVoidMethod(writerObj_, writeMethod_, env_->NewStringUTF(outstream_.str().c_str()));
            // clear the stringstream object
            outstream_.str("");
            outstream_.clear();
        }
#else
#endif
    } else {
        outstreamfs_.flush();
    } 
}
