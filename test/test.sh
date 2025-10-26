#!/bin/bash
set -e
PASS_PATH="../llvm-pass/build/libMySROA.so"
CLANG="clang-14"
OPT="opt-14"
TESTS=("example1" "example2" "example3" "example4")

echo "=== Running MySROA tests ==="
echo

for t in "${TESTS[@]}"; do
  SRC="${t}.c"
  LL="${t}.ll"
  OUT="${t}_out.ll"

  if [ ! -f "$SRC" ]; then
    echo "Skipping $SRC"
    echo
    continue
  fi

  echo "------------------------------------------------------------"
  echo "Test: $SRC"
  echo "------------------------------------------------------------"

  $CLANG -O0 -Xclang -disable-O0-optnone -emit-llvm -S -g -fno-discard-value-names "$SRC" -o "$LL"
  $OPT -enable-new-pm=0 -load "$PASS_PATH" -mysroa -S "$LL" -o "$OUT"

  echo "Generated: $OUT"
  echo "Lines before: $(wc -l < "$LL"), after: $(wc -l < "$OUT")"
  echo
done

echo "=== All tests completed ==="
