# Andrew Yang
# EUID: ay0123
# ID: 11229926
# CSCE 3600 Minor1

BEGIN {
	FS = ","
	nr_student = 0
	nr_test = 3
	total_score = 0
	printf "Student Average Test Scores\n"
	printf "%-15s %s\n", "Name", "Average"
}
NR>1 {
	if ($3 != "A")
		next

	nr_student += 1
	__total_score = $4 + $5 + $6
	total_score += __total_score
	printf "%-15s %.1f\n", $2" "$1, __total_score / nr_test
}
END {
	avg_score = total_score / (nr_test * nr_student)
	printf "%d Students with average score of %.1f\n", nr_student, avg_score
}

