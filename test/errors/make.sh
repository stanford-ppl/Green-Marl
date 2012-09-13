#!/bin/bash
FILES=*.gm
for f in $FILES
do
   echo "doing $f"
   cat $f > output/$f.txt
   ../../bin/gm_comp -DS=3 -V=1 $f >> output/$f.txt

   ## return code should be 0 or 1
   rc=$?
   if [[ $rc != 0 ]] && [[ $rc != 1 ]] ; then
       exit $rc
   fi
done
# Return with 0 (== success)
exit 0
