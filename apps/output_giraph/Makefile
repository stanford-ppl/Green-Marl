
include ../../setup.mk

.PHONY: lib bin target 
GIRAPH_JAR=giraph-0.2-SNAPSHOT-for-hadoop-0.20.203.0-jar-with-dependencies.jar
#HADOOP_JAR=hadoop-core-1.0.3.jar
GM_JAR=gm_lib.jar
TARGET_JAR=gm_apps.jar
GM_JARSOURCES := $(wildcard java_src/*.java)

ifeq ($(CREATE_JAR_FOR_EXAMPLE_GIRAPH_APPS),1)
all: target
else
all: lib
endif

# do nothing
lib: lib/$(GM_JAR)

lib/$(GM_JAR): $(GM_JARSOURCES)
	javac java_src/*.java -d java_bin/ -classpath lib/$(GIRAPH_JAR):$(HADOOP_HOME)/$(HADOOP_CORE_JAR)
	jar cf lib/$(GM_JAR) -C java_bin/ .

bin: lib/$(GIRAPH_JAR) lib/$(GM_JAR)
	javac generated/*.java -d bin/ -classpath lib/$(GM_JAR):lib/$(GIRAPH_JAR):$(HADOOP_HOME)/$(HADOOP_CORE_JAR)

target: bin
	jar cf target/$(TARGET_JAR) -C bin/ .


clean:
	rm -rf bin/*
	rm -rf package/*

clean_all:
	rm -rf lib/$(GM_JAR)
	rm -rf bin/*
	rm -rf package/*
