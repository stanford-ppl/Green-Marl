import java.io.BufferedReader;
import java.io.InputStreamReader;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

/*
 * A Java class that reads one line at a time from a file in HDFS
 */
public class HDFSLineReader {
	String fileName;
	BufferedReader in;
    FileSystem fs;
    Path filePath;
    FSDataInputStream fdis;
	
    // Constructor and initialization
	public HDFSLineReader (String s) {
		try {
			fileName = s;
			Configuration conf = new Configuration();
			conf.addResource(new Path("/cm/shared/apps/hadoop/current/conf/core-site.xml"));
			conf.addResource(new Path("/cm/shared/apps/hadoop/current/conf/hdfs-site.xml"));
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

    // A sample usage for HDFSLineReader
    //     - Only used for debugging purposes
    public static void main(String[] args) {
        if (args.length < 1) {
            System.out.println ("Usage: java HDFSLineReader <inputFile>");
            System.exit(1);
        }
        HDFSLineReader lr = new HDFSLineReader(args[0]);
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
