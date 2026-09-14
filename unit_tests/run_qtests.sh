#!/bin/bash

set -e
set -o pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

BUILD_DIR="$SCRIPT_DIR/build"
OUTPUT_FILE="$SCRIPT_DIR/qtest_results.txt"

echo "=============================================="
echo " Mafija - QTest"
echo "=============================================="
echo

echo "Brisanje prethodnog build direktorijuma..."
rm -rf "$BUILD_DIR"

echo "Brisanje prethodnog izvestaja..."
rm -f "$OUTPUT_FILE"

echo
echo "=============================================="
echo " CMake konfiguracija"
echo "=============================================="
echo

cmake \
    -S "$SCRIPT_DIR" \
    -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Debug \
    2>&1 | tee -a "$OUTPUT_FILE"

echo
echo "=============================================="
echo " Build"
echo "=============================================="
echo

cmake \
    --build "$BUILD_DIR" \
    -j \
    2>&1 | tee -a "$OUTPUT_FILE"

echo
echo "=============================================="
echo " Pokretanje QTest testova"
echo "=============================================="
echo

if ctest \
    --test-dir "$BUILD_DIR" \
    -V \
    2>&1 | tee -a "$OUTPUT_FILE"
then
    TEST_STATUS=0
else
    TEST_STATUS=$?
fi

echo
echo "=============================================="
echo " QTest testovi zavrseni"
echo "=============================================="
echo
echo "Rezultati su sacuvani u:"
echo "$OUTPUT_FILE"

exit "$TEST_STATUS"
