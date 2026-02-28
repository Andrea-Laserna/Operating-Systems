#!/bin/sh
set -e

run_case() {
	title="$1"
	shift
	printf "\n# %s\n" "$title"
	set +e
	sh -c "$*"
	status=$?
	set -e
	printf "(exit=%d)\n" "$status"
}

# Normal case
echo "hello from mysh" > mysh_out.txt
cat < mysh_out.txt
wc -w < mysh_out.txt

# Edge cases
run_case "missing input file" "cat < no_such_file.txt"

run_case "overwrite output" "echo first > tmp_out.txt; echo second > tmp_out.txt; cat tmp_out.txt"

run_case "append output" "echo start > tmp_out.txt; echo more >> tmp_out.txt; cat tmp_out.txt"

run_case "empty input file" ": > empty.txt; wc -w < empty.txt"
