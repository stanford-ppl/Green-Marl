#!/bin/bash
FILES=../../gps/gps_errors/*.gm
for f in $FILES
do
   echo "doing $f"
   cat $f > output/$f.txt
   ../../../bin/gm_comp -DS=7 -V=1 -t=giraph -o=output/ $f >> output/$f.txt
done
