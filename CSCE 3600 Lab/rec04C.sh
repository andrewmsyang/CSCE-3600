#Andrew Yang 11229926 csce3600 rec04
#!/bin/bash

randomNumb=$(( $RANDOM % 10 + 1 ));

echo -n "Enter a number between 1 and 10:"

read input

while [ $input -ne $randomNumb ]

do

   echo "Sorry, you are not correct. Try again!"

   echo -n "Enter a number between 1 and 10:"

   read input

done

echo "Congratulations, the number is $input"
