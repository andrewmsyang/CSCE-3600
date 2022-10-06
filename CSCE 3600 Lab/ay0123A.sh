#!/bin/bash
# Name: Andrew Yang
# Course: CSCE3600.001
# Date: 2/24/2020

addMem(){ 
	echo "USER	%MEM"
	for user in "$@"
	do
		if [ "$np" -ne 0 ]; then
			printf "%-16s %16d\n" "$user" "$np"

                fi
 	done

}
nr_proc() {
	if [ "$#" -lt 1 ]; then
		return
	fi
	ps -u "$1" --no-headers 2>/dev/null | awk 'END {print NR}'
}


if [ "$#" -eq 0 ]; then
        mapfile -t user < <(ps -Ao user --no-headers | sort -u)
else
        userlist=("$@")
fi

while true; do
        addMem "${userlist[@]}"
done

