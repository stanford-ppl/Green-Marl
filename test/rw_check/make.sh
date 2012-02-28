#!/bin/bash
FILES=*.gm
for f in $FILES
do
   echo "doing $f"
   cat $f > output/$f.txt
   ../../bin/gm_comp -DS=2 -Dr -Dw -V=1 $f >> output/$f.txt
done
