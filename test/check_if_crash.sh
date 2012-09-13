#!/bin/bash
#-----------------------------------------------------------
# run clean and make in subdirectorie
#---------------------------------------------------------
set -e
DIRS="parse sugars opt cpp_be gps giraph"
for d in $DIRS
do
  { cd $d; make clean; make; cd ..; }
done

#---------------------------------------------------
# Following directories contain intended 'errors'
# (But should not crash)
#---------------------------------------------------
set +e
DIRS="rw_check errors"
for d in $DIRS
do
  { cd $d; ./clean.sh; 
    ./make.sh; 
   rc=$?;
   if [[ $rc != 0 ]] ; then
       echo ERROR
       exit $rc
   fi;
   cd ..; }
done

echo OKAY
