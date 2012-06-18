#!/bin/bash
OJB=obj
BIN=bin
CPP=apps/output_cpp
LIB=apps/output_cpp/gm_graph
GPS=apps/output_gps
GIRAPH=apps/output_giraph
BUILD_DIRS="bin obj $CPP/bin $CPP/generated $CPP/bin $CPP/data $LIB/lib $GPS/generated $GIRAPH/generated"
for D in $BUILD_DIRS
do
    if [ ! -d  ${D} ]
    then
        mkdir ${D}
    fi

done

TEST_DIRS="cpp_be errors gps giraph opt parse rw_check sugars"
for D in $TEST_DIRS
do
    if [ ! -d  test/${D}/output ]
    then
        mkdir "test/${D}/output"
    fi

done


