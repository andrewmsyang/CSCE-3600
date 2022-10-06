#Andrew Yang ay0123 11229926 rec05.sh
#!/bin/bash

curve(){
   incr=$1
   shift

   arr=("$@")
   index=0

   for i in "${arr[@]}";
   do
       let "grades[$index]=$incr+i"
       index=`expr $index + 1`
   done
}

if [ "$#" != 1 ]
then   
   echo "usage: ./rec05.sh <curve amount>"
else
   declare grades
   	for i in {1..5}
   	do
   		read -p "Enter QUIZ #$i: " grades[$i-1]
   	done

   curve $1 "${grades[@]}"
  	echo "Curved Grades:"
  
   for ((i=1; i<=5; i++));
   do
   echo "grades[$i] = ${grades[$i-1]}"
   done
fi
