#!/bin/bash
set -e


cd "$(dirname "$0")"


SROA_PASS="../llvm-pass/build/libMySROA.so"
MERGE_PASS="../llvm-pass/build/libMergeFunc.so"
CLANG="clang-14"
OPT="opt-14"

echo
echo "=========================================="
echo "=== Running LLVM Pass Tests (MySROA + MyMergeFunc) ==="
echo "=========================================="
echo



run_sroa_test() {
  local SRC="$1"
  local LL="${SRC%.c}.ll"
  local OUT="${SRC%.c}_sroa_out.ll"

  echo "------------------------------------------------------------"
  echo "MySROA Test: $SRC"
  echo "------------------------------------------------------------"
  $CLANG -O0 -Xclang -disable-O0-optnone -emit-llvm -S -g -fno-discard-value-names "$SRC" -o "$LL"
  $OPT -enable-new-pm=0 -load "$SROA_PASS" -mysroa -S "$LL" -o "$OUT"
  echo "Generated: $OUT"
  echo "Lines before: $(wc -l < "$LL"), after: $(wc -l < "$OUT")"
  echo
}

run_merge_test() {
  local SRC="$1"
  local LL="${SRC%.c}.ll"
  local OUT="${SRC%.c}_merge_out.ll"

  echo "------------------------------------------------------------"
  echo "MyMergeFunc Test: $SRC"
  echo "------------------------------------------------------------"
  $CLANG -O0 -Xclang -disable-O0-optnone -emit-llvm -S -g -fno-discard-value-names "$SRC" -o "$LL"
  $OPT -enable-new-pm=0 -load "$MERGE_PASS" -mymergefunc -S "$LL" -o "$OUT"
  echo "Generated: $OUT"
  echo "Lines before: $(wc -l < "$LL"), after: $(wc -l < "$OUT")"
  echo
}

run_combined_test() {
  local SRC="$1"
  local LL="${SRC%.c}.ll"
  local OUT="${SRC%.c}_combined_out.ll"

  echo "------------------------------------------------------------"
  echo "Combined Pass Test (MyMergeFunc + MySROA): $SRC"
  echo "------------------------------------------------------------"
  $CLANG -O0 -Xclang -disable-O0-optnone -emit-llvm -S -g -fno-discard-value-names "$SRC" -o "$LL"

  
  $OPT -enable-new-pm=0 \
       -load "$MERGE_PASS" -mymergefunc \
       -load "$SROA_PASS" -mysroa \
       -S "$LL" -o "$OUT"

  echo "Generated: $OUT"
  echo "Lines before: $(wc -l < "$LL"), after: $(wc -l < "$OUT")"
  echo
}




echo "=== Running MySROA tests (test/sroa/) ==="
for SRC in sroa/*.c; do
  [ -f "$SRC" ] && run_sroa_test "$SRC"
done

echo "=== Running MyMergeFunc tests (test/merge/) ==="
for SRC in merge/*.c; do
  [ -f "$SRC" ] && run_merge_test "$SRC"
done

echo "=== Running Combined tests (test/combined/) ==="
for SRC in combined/*.c; do
  [ -f "$SRC" ] && run_combined_test "$SRC"
done

echo "=========================================="
echo "All tests completed successfully!"
echo "=========================================="
