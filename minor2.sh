#!/bin/bash
# Name: Andrew Yang
# Course: CSCE3600.001
# Date: 2/23/2020
# Description: This monitors the number of processes per user on current machine
# trap and handle SIGINT

sigint_handler() {
	echo " (SIGINT) received"
	read -p ' Terminate Program? (Y/N)' answer
		case $answer in
		[yY])
			echo "Terminating program"
			kill -TERM -$$
			;;
		*)
			echo "Continuing Execution"
			;;
		esac
}

# register custom SIGINT handler
trap sigint_handler INT

nr_proc() {
	if [ "$#" -lt 1 ]; then
		return
	fi
	ps -u "$1" --no-headers 2>/dev/null | awk 'END {print NR}'
}

# report user and number pf process once
report() {
	echo
	date +"%a %d %b %Y %I:%M:%S %p %Z"
	printf "%-16s %16s\n" "User ID" "Count"

	total_proc=0
	for user in "$@"; do
		np=$(nr_proc "$user")
		if [ "$np" -ne 0 ]; then
			printf "%-16s %16d\n" "$user" "$np"
			total_proc=$((total_proc + np))
		fi
	done

	printf "%3d USERS, TOTAL PROCESSES %6d\n" "$#" "$total_proc"
}

# the main routine
# NOTE we use "ps -A" instead of "ps -a" to find all users, including system roles
if [ "$#" -eq 0 ]; then
	mapfile -t userlist < <(ps -Ao user --no-headers | sort -u)
else
	userlist=("$@")
fi

while true; do
	report "${userlist[@]}"
	sleep 5
done

