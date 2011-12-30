#!/bin/bash
FILES=*.gm
for f in $FILES
do
   echo "doing $f"
   cat $f > output/$f.txt
   ./gm_comp -DS=3 -V=1 $f >> output/$f.txt
done
