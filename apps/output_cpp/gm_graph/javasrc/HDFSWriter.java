import java.io.BufferedWriter;
import java.io.OutputStreamWriter;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

public class HDFSWriter {
	String fileName;
	BufferedWriter out;
    FileSystem fs;
    Path filePath;
    FSDataOutputStream fdos;
	
	public HDFSWriter (String s) {
		try {
			fileName = s;
			Configuration conf = new Configuration();
			conf.addResource(new Path(System.getenv("HADOOP_HOME") + "/conf/core-site.xml"));
			conf.addResource(new Path(System.getenv("HADOOP_HOME") + "/conf/hdfs-site.xml"));
			fs = FileSystem.get(conf);
		    filePath = new Path(fileName);
			fdos = fs.create(filePath);
			out = new BufferedWriter(new OutputStreamWriter(fdos));
		} catch (Exception e) {
			System.err.println (e);
		}
	}

    public void write (String str) {
        try {
            out.write (str, 0, str.length());
            out.flush ();
        } catch (Exception e) {
            System.err.println (e);
        }
    }
	
    public void terminate() {
        try {
            fdos.close();
            fs.close();
        } catch (Exception e) {
            System.err.println (e);
        }
    }

    public static void main(String[] args) {
        if (args.length < 1) {
            System.out.println ("Usage: java HDFSWriter <outputFile>");
            System.exit(1);
        }
        HDFSWriter writer = new HDFSWriter(args[0]);
        for (int i = 0; i < 10; i++) {
            String str = " " + i + " " + 2 * (i+2) + " " + i * 10 + "\n";
            System.out.print ("Writing: " + str);
            writer.write(str);
        }
        writer.terminate();
    }
}

