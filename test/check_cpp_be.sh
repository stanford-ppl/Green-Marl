#!/bin/bash
#-----------------------------------------------------------
# compile cpp_be and 
#---------------------------------------------------------
set -e
DIRS="cpp_be"
for d in $DIRS
do
  { cd $d; make $@; make dummy $@; cd ..; }
done

