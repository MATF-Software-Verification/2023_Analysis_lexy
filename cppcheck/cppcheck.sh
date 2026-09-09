#!/bin/bash

set -eu
set -o pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MAFIJA_DIR="$(cd "$SCRIPT_DIR/../Mafija" && pwd)"

BUILD_DIR="$SCRIPT_DIR/build"
REPORT_FILE="$SCRIPT_DIR/cppcheck-report.txt"

COMPILE_COMMANDS="$BUILD_DIR/compile_commands.json"
FILTERED_COMPILE_COMMANDS="$BUILD_DIR/compile_commands_filtered.json"

echo "=============================================="
echo "             Cppcheck - Mafija"
echo "=============================================="
echo



echo "Provera alata..."

if ! command -v cppcheck >/dev/null 2>&1; then
    echo "Greska: cppcheck nije instaliran."
    echo "Instalacija: sudo apt install cppcheck"
    exit 1
fi

if ! command -v cmake >/dev/null 2>&1; then
    echo "Greska: cmake nije instaliran."
    exit 1
fi

if ! command -v python3 >/dev/null 2>&1; then
    echo "Greska: python3 nije instaliran."
    exit 1
fi

echo "Cppcheck: $(which cppcheck)"
cppcheck --version

echo "CMake: $(which cmake)"
cmake --version | head -n 1

echo "Python: $(which python3)"
python3 --version

echo



if [ ! -d "$MAFIJA_DIR" ]; then
    echo "Greska: Mafija projekat nije pronadjen:"
    echo "$MAFIJA_DIR"
    exit 1
fi

echo "Mafija projekat:"
echo "$MAFIJA_DIR"
echo



echo "=============================================="
echo "Ciscenje prethodnih rezultata"
echo "=============================================="

rm -rf "$BUILD_DIR"
rm -f "$REPORT_FILE"

mkdir -p "$BUILD_DIR"

echo "Build direktorijum:"
echo "$BUILD_DIR"
echo



echo "=============================================="
echo "CMake configure"
echo "=============================================="

cmake \
    -S "$MAFIJA_DIR" \
    -B "$BUILD_DIR" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

echo


echo "=============================================="
echo "CMake build"
echo "=============================================="

cmake \
    --build "$BUILD_DIR" \
    --parallel "$(nproc)"

echo



if [ ! -f "$COMPILE_COMMANDS" ]; then
    echo "Greska: compile_commands.json nije generisan."
    exit 1
fi

echo "Pronadjen compilation database:"
echo "$COMPILE_COMMANDS"
echo



echo "=============================================="
echo "Filtriranje compile_commands.json"
echo "=============================================="

python3 - "$COMPILE_COMMANDS" \
         "$FILTERED_COMPILE_COMMANDS" \
         "$MAFIJA_DIR" <<'PY'

import json
import os
import sys

source_db = sys.argv[1]
filtered_db = sys.argv[2]
mafija_dir = os.path.realpath(sys.argv[3])

# Direktorijum sa testovima koji ne zelimo da analiziramo.
tests_dir = os.path.realpath(
    os.path.join(mafija_dir, "tests")
)

with open(source_db, "r", encoding="utf-8") as f:
    commands = json.load(f)

filtered = []

for command in commands:

    file_path = command.get("file")

    if not file_path:
        continue

    # Ako je putanja relativna, vezujemo je za
    # direktorijum iz compilation database-a.
    if not os.path.isabs(file_path):
        directory = command.get("directory", mafija_dir)
        file_path = os.path.join(directory, file_path)

    file_path = os.path.realpath(file_path)



    if not (
        file_path == mafija_dir
        or file_path.startswith(mafija_dir + os.sep)
    ):
        continue



    if (
        file_path == tests_dir
        or file_path.startswith(tests_dir + os.sep)
    ):
        continue



    command["file"] = file_path

    filtered.append(command)

with open(filtered_db, "w", encoding="utf-8") as f:
    json.dump(filtered, f, indent=2)

print(f"Originalnih komandi: {len(commands)}")
print(f"Komandi za Mafija projekat: {len(filtered)}")

PY

echo

if [ ! -f "$FILTERED_COMPILE_COMMANDS" ]; then
    echo "Greska: filtrirani compilation database nije generisan."
    exit 1
fi



echo "=============================================="
echo "Cppcheck analiza"
echo "=============================================="

echo
echo "Analizira se produkcioni kod Mafija projekta."
echo
echo "Iskljuceno:"
echo "  - tests/"
echo "  - CMake generisani fajlovi"
echo "  - _autogen/"
echo "  - _deps/"
echo "  - Catch2"
echo "  - style provere"
echo

cppcheck \
    --project="$FILTERED_COMPILE_COMMANDS" \
    --library=qt \
    --enable=warning,performance,portability \
    --disable=style \
    --inconclusive \
    2>&1 | tee "$REPORT_FILE"

echo



echo "=============================================="
echo "Analiza zavrsena"
echo "=============================================="

echo
echo "Izvestaj:"
echo "$REPORT_FILE"

echo
echo "Filtrirani compilation database:"
echo "$FILTERED_COMPILE_COMMANDS"



