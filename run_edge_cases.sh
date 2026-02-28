#!/bin/sh
set -e

if [ ! -x ./mysh ]; then
  printf "./mysh not found or not executable\n" >&2
  exit 1
fi

run_case() {
  title=$1
  file=$2
  printf "===== %s =====\n" "$title"
  set +e
  ./mysh <"$file"
  mysh_status=$?
  /bin/bash <"$file"
  bash_status=$?
  set -e
  printf "mysh exit: %s\n" "$mysh_status"
  printf "bash exit: %s\n\n" "$bash_status"
}

tmp1=$(mktemp)
tmp2=$(mktemp)
tmp3=$(mktemp)
tmp4=$(mktemp)
tmp5=$(mktemp)
tmp6=$(mktemp)

printf 'echo %04000d 0\n' 0 >"$tmp1"

i=1
args=""
while [ "$i" -le 300 ]; do
  args="$args arg$i"
  i=$((i + 1))
done
printf 'echo%s\n' "$args" >"$tmp2"

printf 'echo one\necho two\necho three\n' >"$tmp3"

cat >"$tmp4" <<'EOF'
sleep 0.1 &
echo after background
sleep 0.3
EOF

cat >"$tmp5" <<'EOF'
echo hidden > /dev/null
echo visible
EOF

cat >"$tmp6" <<'EOF'
ls > ls_output.txt
cat ls_output.txt
EOF

run_case "01: long command line" "$tmp1"
run_case "02: many arguments" "$tmp2"
run_case "03: rapid commands" "$tmp3"
run_case "04: background job" "$tmp4"
run_case "05: redirection to /dev/null" "$tmp5"
run_case "06: spaced redirection" "$tmp6"

if [ -f ls_output.txt ]; then
  printf "===== ls_output.txt report =====\n"
  ls -l ls_output.txt
  printf "%s\n" "--- contents ---"
  cat ls_output.txt
  printf "\n"
fi

rm -f "$tmp1" "$tmp2" "$tmp3" "$tmp4" "$tmp5" "$tmp6"
