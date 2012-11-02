#ifndef GM_FILE_HANDLING_H_
#define GM_FILE_HANDLING_H_

#include <string>
#include <fstream>

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
    std::ifstream fs_;
    bool failed_;
};


#endif /* GM_FILE_HANDLING_H_ */
