#!/bin/bash

set -eu


# Podesavanja

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

MAFIJA_DIR="$SCRIPT_DIR/../Mafija"

BUILD_DIR="$SCRIPT_DIR/build"


RAW_OUTPUT="$SCRIPT_DIR/izlaz.txt"
ENVIRONMENT="$SCRIPT_DIR/okruzenje.txt"


CLANG="${CLANG:-clang}"
SCAN_BUILD="${SCAN_BUILD:-scan-build}"
SCAN_VIEW="${SCAN_VIEW:-scan-view}"


echo "=========================================="
echo " Clang Static Analyzer - Mafija"
echo "=========================================="


echo
echo "Provera alata..."

command -v cmake >/dev/null 2>&1 || {
    echo "ERROR: cmake nije pronadjen."
    exit 1
}

command -v "$CLANG" >/dev/null 2>&1 || {
    echo "ERROR: clang nije pronadjen:"
    echo "$CLANG"
    exit 1
}

command -v "$SCAN_BUILD" >/dev/null 2>&1 || {
    echo "ERROR: scan-build nije pronadjen:"
    echo "$SCAN_BUILD"
    exit 1
}

SCAN_VIEW_AVAILABLE=1

if ! command -v "$SCAN_VIEW" >/dev/null 2>&1; then
    SCAN_VIEW_AVAILABLE=0
fi



CLANG_PATH="$(command -v "$CLANG")"

if command -v clang++ >/dev/null 2>&1; then
    CLANGXX="clang++"
    CLANGXX_PATH="$(command -v clang++)"
else
    echo
    echo "ERROR: clang++ nije pronadjen."
    exit 1
fi

SCAN_BUILD_PATH="$(command -v "$SCAN_BUILD")"

if [ "$SCAN_VIEW_AVAILABLE" -eq 1 ]; then
    SCAN_VIEW_PATH="$(command -v "$SCAN_VIEW")"
else
    SCAN_VIEW_PATH=""
fi



echo
echo "Clang:"
echo "$CLANG_PATH"
"$CLANG_PATH" --version

echo
echo "Clang++:"
echo "$CLANGXX_PATH"
"$CLANGXX_PATH" --version

echo
echo "scan-build:"
echo "$SCAN_BUILD_PATH"
"$SCAN_BUILD_PATH" --help | head -n 3

echo
echo "scan-view:"
if [ "$SCAN_VIEW_AVAILABLE" -eq 1 ]; then
    echo "$SCAN_VIEW_PATH"
else
    echo "nije pronadjen"
fi



CLANG_VERSION="$("$CLANG_PATH" --version | head -n 1)"

echo
echo "Koristi se:"
echo "$CLANG_VERSION"



if [ ! -f "$MAFIJA_DIR/CMakeLists.txt" ]; then
    echo
    echo "ERROR: CMakeLists.txt nije pronadjen:"
    echo "$MAFIJA_DIR"
    exit 1
fi


echo
echo "=========================================="
echo " Ciscenje prethodnih rezultata"
echo "=========================================="

rm -rf "$BUILD_DIR"

find "$SCRIPT_DIR" \
    -mindepth 1 \
    -maxdepth 1 \
    -type d \
    -name 'scan-build-*' \
    -exec rm -rf {} +

rm -f "$RAW_OUTPUT"
rm -f "$ENVIRONMENT"

mkdir -p "$BUILD_DIR"


echo
echo "=========================================="
echo " CMake configure"
echo "=========================================="

cmake \
    -S "$MAFIJA_DIR" \
    -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_C_COMPILER="$CLANG_PATH" \
    -DCMAKE_CXX_COMPILER="$CLANGXX_PATH" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON



echo
echo "=========================================="
echo " Clang Static Analyzer"
echo "=========================================="

set +e

"$SCAN_BUILD_PATH" \
    -o "$SCRIPT_DIR" \
    -enable-checker core \
    -enable-checker unix \
    -enable-checker security \
    -enable-checker deadcode \
    -v \
    cmake --build "$BUILD_DIR" --parallel \
    2>&1 | tee "$RAW_OUTPUT"

ANALYSIS_STATUS=${PIPESTATUS[0]}

set -e



{
    echo "Clang Static Analyzer - Mafija"
    echo

    echo "=========================================="
    echo "Datum"
    echo "=========================================="
    date

    echo
    echo "=========================================="
    echo "Projekat"
    echo "=========================================="
    echo "$MAFIJA_DIR"

    echo
    echo "=========================================="
    echo "Build direktorijum"
    echo "=========================================="
    echo "$BUILD_DIR"

    echo
    echo "=========================================="
    echo "Clang"
    echo "=========================================="
    echo "Putanja:"
    echo "$CLANG_PATH"
    echo
    "$CLANG_PATH" --version

    echo
    echo "=========================================="
    echo "Clang++"
    echo "=========================================="
    echo "Putanja:"
    echo "$CLANGXX_PATH"
    echo
    "$CLANGXX_PATH" --version

    echo
    echo "=========================================="
    echo "scan-build"
    echo "=========================================="
    echo "Putanja:"
    echo "$SCAN_BUILD_PATH"
    echo
    "$SCAN_BUILD_PATH" --help | head -n 3

    echo
    echo "=========================================="
    echo "scan-view"
    echo "=========================================="
    if [ "$SCAN_VIEW_AVAILABLE" -eq 1 ]; then
        echo "$SCAN_VIEW_PATH"
    else
        echo "nije pronadjen"
    fi

    echo
    echo "=========================================="
    echo "Checkeri"
    echo "=========================================="
    echo "core"
    echo "unix"
    echo "security"
    echo "deadcode"

    echo
    echo "=========================================="
    echo "CMake build type"
    echo "=========================================="
    echo "Debug"

    echo
    echo "=========================================="
    echo "Status analize"
    echo "=========================================="
    echo "$ANALYSIS_STATUS"

} > "$ENVIRONMENT"



echo
echo "=========================================="
echo " Analiza zavrsena"
echo "=========================================="

echo
echo "Rezultati se nalaze u:"
echo "$SCRIPT_DIR"

echo
echo "Sirovi izlaz:"
echo "$RAW_OUTPUT"

echo
echo "Informacije o okruzenju:"
echo "$ENVIRONMENT"



if [ "$ANALYSIS_STATUS" -ne 0 ]; then

    echo
    echo "=========================================="
    echo " GRESKA"
    echo "=========================================="

    echo
    echo "Build ili Clang Static Analyzer nisu"
    echo "uspesno zavrseni."

    echo
    echo "Status:"
    echo "$ANALYSIS_STATUS"

    echo
    echo "Detalji se nalaze u:"
    echo "$RAW_OUTPUT"

    exit "$ANALYSIS_STATUS"
fi



REPORT="$(find "$SCRIPT_DIR" \
    -mindepth 1 \
    -maxdepth 1 \
    -type d \
    -name 'scan-build-*' \
    -print -quit)"



if [ -n "$REPORT" ]; then

    echo
    echo "=========================================="
    echo " Pronadjen scan-build izvestaj"
    echo "=========================================="

    echo
    echo "$REPORT"

    if [ "$SCAN_VIEW_AVAILABLE" -eq 1 ]; then

        echo
        echo "Pokrecem scan-view..."

        "$SCAN_VIEW_PATH" "$REPORT"

    else

        echo
        echo "scan-view nije pronadjen."

        echo
        echo "Izvestaj mozete otvoriti rucno:"
        echo "$REPORT"

    fi

else

    echo
    echo "=========================================="
    echo " Nema prijavljenih problema"
    echo "=========================================="

    echo
    echo "Clang Static Analyzer nije prijavio"
    echo "probleme."

    echo
    echo "Sirovi izlaz:"
    echo "$RAW_OUTPUT"

fi

