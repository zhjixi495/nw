# Author: ZJX
#!/bin/bash
find ./ -name "*.txt" | while read fname 
do 
   
   diff $fname ${fname%.txt}
   if [ $? -ne 0 ]
   then
   echo "$fname";
   cp $fname ${fname%.txt}
   fi
done
