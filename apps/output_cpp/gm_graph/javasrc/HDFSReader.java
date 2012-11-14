import java.io.BufferedReader;
import java.io.InputStreamReader;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

/*
 * A Java class that reads one line at a time from a file in HDFS
 */
public class HDFSReader {
    String fileName;
    BufferedReader in;
    FileSystem fs;
    Path filePath;
    FSDataInputStream fdis;

    // Constructor and initialization
    public HDFSReader (String s) {
        try {
            fileName = s;
            Configuration conf = new Configuration();
            conf.addResource(new Path(System.getenv("HADOOP_HOME") + "/conf/core-site.xml"));
            conf.addResource(new Path(System.getenv("HADOOP_HOME") + "/conf/hdfs-site.xml"));
            fs = FileSystem.get(conf);
            filePath = new Path(fileName);
            if ( ! fs.exists(filePath)) {
                System.out.println ("File does not exist: " + fileName);
                return;
            }
            fdis = fs.open(filePath);
            in = new BufferedReader(new InputStreamReader(fdis));
        } catch (Exception e) {
            System.err.println (e);
        }
    }

    // Get the next line from the file
    public String getLine() {
        try {
            return in.readLine();
        } catch (Exception e) {
            System.err.println (e);
        }
        return null;
    }

    // Get the a sequence of bytes from the file
    public byte[] getBytes(int num_bytes) {
        try {
            byte[] ret = new byte[num_bytes];
            int bytes_read = fdis.read(ret);
            if (bytes_read != num_bytes && bytes_read > 0) {
                // as far as I can tell the only way to handle EOF
                byte[] ret_short  = new byte[bytes_read];
                System.arraycopy(ret, 0, ret_short, 0, bytes_read);
                ret = ret_short;
            }
            return ret;
        } catch (Exception e) {
            System.err.println (e);
        }
        return null;
    }

    // Set postion in the file from a given position
    public int seekCurrent(long pos) {
        try {
            fdis.seek(fdis.getPos()+pos);
            return 0;
        } catch (Exception e) {
            System.err.println (e);
        }
        return -1;
    }

    // Reset the pointer (reader) to the start of the file
    public void reset() {
        try {
            fdis.seek(0);
        } catch (Exception e) {
            System.err.println (e);
        }
    }

    // Close the input stream and the hdfs file system
    public void terminate() {
        try {
            fdis.close();
            fs.close();
        } catch (Exception e) {
            System.err.println (e);
        }
    }

    // A sample usage for HDFSReader
    //     - Only used for debugging purposes
    public static void main(String[] args) {
        if (args.length < 1) {
            System.out.println ("Usage: java HDFSReader <inputFile>");
            System.exit(1);
        }
        HDFSReader lr = new HDFSReader(args[0]);
        String str;
        while ((str = lr.getLine()) != null) {
            System.out.println (str);
        }
        lr.reset();
        System.out.println ("Reseting the graph to the start...");
        while ((str = lr.getLine()) != null) {
            System.out.println (str);
        }
        lr.terminate();
    }
}
