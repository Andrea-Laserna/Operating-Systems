#!/bin/sh
set -e

rm -f mysh_out.txt tmp_out.txt empty.txt

run_case() {
  title=$1
  printf "===== %s =====\n" "$title"
  tmp_input=$(mktemp)
  cat >"$tmp_input"
  set +e
  ./mysh <"$tmp_input"
  rc=$?
  set -e
  printf "exit=%s\n\n" "$rc"
  rm -f "$tmp_input"
}

run_case "01: normal echo/cat/wc to mysh_out.txt" <<'EOF'
echo "hello world" > mysh_out.txt
cat mysh_out.txt
wc -w < mysh_out.txt
EOF

run_case "02: missing input redirection" <<'EOF'
cat < missing.txt
EOF

run_case "03: overwrite then cat tmp_out.txt" <<'EOF'
echo "first line" > tmp_out.txt
echo "second line" > tmp_out.txt
cat tmp_out.txt
EOF

run_case "04: append to tmp_out.txt" <<'EOF'
echo "append A" >> tmp_out.txt
echo "append B" >> tmp_out.txt
cat tmp_out.txt
EOF

run_case "05: wc on empty file" <<'EOF'
: > empty.txt
wc -w < empty.txt
EOF

run_case "06: malformed redirection" <<'EOF'

cat <
EOF

for f in mysh_out.txt tmp_out.txt empty.txt; do
  if [ -e "$f" ]; then
    ls -l "$f"
    printf "=== %s contents ===\n" "$f"
    cat "$f"
    printf "\n"
  else
    printf "=== %s (not found) ===\n" "$f"
  fi
done
