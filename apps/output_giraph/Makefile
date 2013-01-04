
include ../../setup.mk

.PHONY: lib bin target 
GIRAPH_JAR=giraph-0.2-SNAPSHOT-for-hadoop-0.20.203.0-jar-with-dependencies.jar
#HADOOP_JAR=hadoop-core-1.0.3.jar
TARGET_JAR=gm_apps.jar

ifeq ($(CREATE_JAR_FOR_EXAMPLE_GIRAPH_APPS),1)
all: target
else
all: lib
endif

# do nothing
lib: 

bin: lib/$(GIRAPH_JAR) 
	javac generated/*.java -d bin/ -classpath lib/$(GIRAPH_JAR):$(HADOOP_HOME)/$(HADOOP_CORE_JAR)

target: bin
	jar cf target/$(TARGET_JAR) -C bin/ .


clean:
	rm -rf bin/*
	rm -rf package/*

clean_all:
	rm -rf bin/*
	rm -rf package/*
