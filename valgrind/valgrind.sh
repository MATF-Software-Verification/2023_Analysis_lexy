#!/bin/bash

set -eu
set -o pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

MAFIJA_DIR="$SCRIPT_DIR/../Mafija"
BUILD_DIR="$SCRIPT_DIR/build"

TARGET="$BUILD_DIR/GUI/GUI"

FULL_REPORT="$SCRIPT_DIR/full-report.txt"


echo "=========================================="
echo " Valgrind Memcheck - Mafija"
echo "=========================================="


echo
echo "Provera alata..."


if ! command -v cmake >/dev/null 2>&1; then
    echo "Greska: cmake nije pronadjen."
    exit 1
fi


if ! command -v valgrind >/dev/null 2>&1; then
    echo "Greska: valgrind nije pronadjen."
    exit 1
fi


echo "CMake:    $(command -v cmake)"
echo "Valgrind: $(command -v valgrind)"


echo
echo "Ciscenje prethodnih rezultata..."


rm -rf "$BUILD_DIR"
rm -f "$FULL_REPORT"


echo
echo "CMake configure..."


cmake \
    -S "$MAFIJA_DIR" \
    -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Debug


echo
echo "Build projekta..."


cmake \
    --build "$BUILD_DIR" \
    --parallel "$(nproc)"


echo
echo "Provera executable fajla..."


if [ ! -x "$TARGET" ]; then
    echo "Greska: Mafija executable nije pronadjen:"
    echo "$TARGET"
    exit 1
fi


echo "Executable:"
echo "$TARGET"


echo
echo "=========================================="
echo " Pokretanje Valgrind Memcheck"
echo "=========================================="
echo
echo "Leak check:     full"
echo "Leak kinds:     all"
echo "Track origins:  ukljucen"
echo "Call stack:     30 funkcija"
echo "Error limit:    iskljucen"
echo "Suppressions:   nema"
echo


set +e


valgrind \
    --tool=memcheck \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --num-callers=30 \
    --error-limit=no \
    --error-exitcode=1 \
    "$TARGET" \
    2>&1 | tee "$FULL_REPORT"


VALGRIND_STATUS=${PIPESTATUS[0]}


set -e


echo
echo "=========================================="
echo " Rezultat analize"
echo "=========================================="


echo
echo "Kompletan Valgrind izvestaj:"
echo "  $FULL_REPORT"


if [ "$VALGRIND_STATUS" -ne 0 ]; then
    echo
    echo " Valgrind je prijavio greske."
    echo " Napomena: greske nisu potisnute."
else
    echo
    echo " Valgrind nije prijavio greske."
fi


echo
echo "=========================================="


exit "$VALGRIND_STATUS"
