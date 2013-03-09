
include ../../setup.mk

ODIR = output
COMP = ../../bin/gm_comp
FLAGS = -o=$(ODIR) -t=giraph

SRCS = gps1.gm gps2.gm gps3.gm gps4.gm gps5.gm gps6.gm gps7.gm gps8.gm gps9.gm gps10.gm gps11.gm gps12.gm gps13.gm gps14.gm gps15.gm gps16.gm gps17.gm gps18.gm gps19.gm gps20.gm gps21.gm gps22.gm gps23.gm gps24.gm gps25.gm #gps26.gm
SRCS2 = gps9.gm gps19.gm gps20.gm

OUTS =$(addprefix $(ODIR)/, $(SRCS:.gm=.java)) 
OUTS2 =$(addprefix $(ODIR)/result_, $(SRCS2))
all: $(OUTS) #$(OUTS2)

$(ODIR)/%.java: %.gm
	$(COMP) $(FLAGS) $< 

$(ODIR)/result_%.gm: %.gm
	$(COMP) $(FLAGS) -DS=5 -V=1 $< > $@ 

clean: 
	\rm -rf $(ODIR)/*.java *.java $(ODIR)/*.gm *.cc *.h $(ODIR)/*.jar

GIRAPH_JAR=giraph-0.2-SNAPSHOT-for-hadoop-0.20.203.0-jar-with-dependencies.jar
TEST_JAR = test.jar
jar: output/$(TEST_JAR)

output/$(TEST_JAR): 
	mkdir -p lib
	mkdir -p bin
	javac output/*.java -Xlint -d bin/ -classpath $(GM_TOP)/apps/output_giraph/lib/$(GIRAPH_JAR):$(HADOOP_HOME)/$(HADOOP_CORE_JAR)
	jar cf output/$(TEST_JAR) -C bin/ .
