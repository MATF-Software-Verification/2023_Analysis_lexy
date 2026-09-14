#!/bin/bash
set -eu


SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

MAFIJA_DIR="$SCRIPT_DIR/../Mafija"
BUILD_DIR="$SCRIPT_DIR/build"

REPORT="$SCRIPT_DIR/clang-tidy-report.txt"

COMPILE_COMMANDS="$BUILD_DIR/compile_commands.json"
BACKUP_COMPILE_COMMANDS="$BUILD_DIR/compile_commands.json.original"

FILTER_SCRIPT="$BUILD_DIR/filter_compile_commands.py"




CLANG_TIDY="${CLANG_TIDY:-clang-tidy}"
RUN_CLANG_TIDY="${RUN_CLANG_TIDY:-run-clang-tidy}"
CMAKE="${CMAKE:-cmake}"




CHECKS="bugprone-*,performance-*,modernize-*,readability-*"




echo
echo "============================================================"
echo "        Clang-Tidy analiza projekta Mafija - V4"
echo "============================================================"
echo




if [ ! -d "$MAFIJA_DIR" ]; then
    echo "Greska: Mafija projekat nije pronadjen:"
    echo
    echo "  $MAFIJA_DIR"
    echo
    exit 1
fi

echo "Mafija projekat:"
echo "  $MAFIJA_DIR"
echo




echo "============================================================"
echo "Provera alata"
echo "============================================================"
echo

if ! command -v "$CLANG_TIDY" >/dev/null 2>&1; then
    echo "Greska: clang-tidy nije pronadjen."
    exit 1
fi

if ! command -v "$RUN_CLANG_TIDY" >/dev/null 2>&1; then
    echo "Greska: run-clang-tidy nije pronadjen."
    exit 1
fi

if ! command -v "$CMAKE" >/dev/null 2>&1; then
    echo "Greska: cmake nije pronadjen."
    exit 1
fi

echo "clang-tidy:"
echo "  $(command -v "$CLANG_TIDY")"

echo "run-clang-tidy:"
echo "  $(command -v "$RUN_CLANG_TIDY")"

echo "cmake:"
echo "  $(command -v "$CMAKE")"

echo




echo "============================================================"
echo "Ciscenje prethodne analize"
echo "============================================================"
echo

rm -rf "$BUILD_DIR"
rm -f "$REPORT"

mkdir -p "$BUILD_DIR"

echo "Obrisan prethodni build:"
echo "  $BUILD_DIR"

echo "Obrisan prethodni izvestaj:"
echo "  $REPORT"

echo




echo "============================================================"
echo "CMake configure"
echo "============================================================"
echo

"$CMAKE" \
    -S "$MAFIJA_DIR" \
    -B "$BUILD_DIR" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

echo




echo "============================================================"
echo "CMake build"
echo "============================================================"
echo

"$CMAKE" \
    --build "$BUILD_DIR" \
    -j"$(nproc)"

echo




if [ ! -f "$COMPILE_COMMANDS" ]; then
    echo "Greska:"
    echo "compile_commands.json nije generisan."
    exit 1
fi

echo "compile_commands.json je generisan:"
echo "  $COMPILE_COMMANDS"
echo



cat > "$FILTER_SCRIPT" <<'PYTHON'
import json
import os
import sys


compile_commands = sys.argv[1]
mafia_dir = os.path.abspath(sys.argv[2])


with open(compile_commands, "r", encoding="utf-8") as f:
    commands = json.load(f)


mafia_dir = os.path.realpath(mafia_dir)


allowed_dirs = [
    os.path.join(mafia_dir, "game"),
    os.path.join(mafia_dir, "GUI"),
    os.path.join(mafia_dir, "server_client"),
]


def is_inside(path, directory):
    try:
        return os.path.commonpath([path, directory]) == directory
    except ValueError:
        return False


def is_allowed_source(path):
    path = os.path.realpath(path)

    # Mora biti unutar jednog od zeljenih direktorijuma.
    if not any(is_inside(path, d) for d in allowed_dirs):
        return False

    # Tests ne ulaze u glavnu analizu.
    if is_inside(path, os.path.join(mafia_dir, "tests")):
        return False

    # Build direktorijum ne ulazi u analizu.
    if is_inside(path, os.path.join(mafia_dir, "build")):
        return False

    # Qt/CMake generated fajlovi.
    lower_path = path.lower()

    generated_parts = [
        "_autogen",
        "/autogen/",
        "/cmakefiles/",
        "/cmakefiles/",
    ]

    for part in generated_parts:
        if part in lower_path:
            return False

    # Samo C/C++ source fajlovi.
    extension = os.path.splitext(path)[1].lower()

    if extension not in [".cpp", ".cc", ".cxx"]:
        return False

    return True


filtered = []

for command in commands:
    file_path = command.get("file", "")

    if not os.path.isabs(file_path):
        directory = command.get("directory", "")
        file_path = os.path.join(directory, file_path)

    file_path = os.path.realpath(file_path)

    if is_allowed_source(file_path):
        filtered.append(command)


with open(compile_commands, "w", encoding="utf-8") as f:
    json.dump(filtered, f, indent=2)


print(f"Original translation units: {len(commands)}")
print(f"Filtered translation units: {len(filtered)}")

print()
print("Fajlovi koji ce biti analizirani:")

for command in filtered:
    print("  " + command["file"])
PYTHON




echo "============================================================"
echo "Filtriranje compile_commands.json"
echo "============================================================"
echo

python3 "$FILTER_SCRIPT" \
    "$COMPILE_COMMANDS" \
    "$MAFIJA_DIR"

echo



FILTERED_COUNT="$(
    python3 - "$COMPILE_COMMANDS" <<'PYTHON'
import json
import sys

with open(sys.argv[1], "r", encoding="utf-8") as f:
    data = json.load(f)

print(len(data))
PYTHON
)"

if [ "$FILTERED_COUNT" -eq 0 ]; then
    echo
    echo "GRESKA: Nakon filtriranja nije ostao nijedan source fajl."
    echo
    echo "Proveri compile_commands.json:"
    echo "  $COMPILE_COMMANDS"
    exit 1
fi


echo "Broj fajlova za Clang-Tidy:"
echo "  $FILTERED_COUNT"
echo




echo "============================================================"
echo "Clang-Tidy analiza"
echo "============================================================"
echo



echo "Obuhvaceni direktorijumi:"
echo "  game/"
echo "  GUI/"
echo "  server_client/Shared/"
echo "  server_client/Server/"
echo "  server_client/Client/"
echo

echo "Iskljuceni:"
echo "  tests/"
echo "  build/"
echo "  *_autogen/"
echo




"$RUN_CLANG_TIDY" \
    -p "$BUILD_DIR" \
    -checks="$CHECKS" \
    -header-filter="$MAFIJA_DIR/(game|GUI|server_client)/.*" \
    -extra-arg=-Wno-unknown-warning-option \
    > "$REPORT" 2>&1 || true




echo
echo "============================================================"
echo "Rezultati analize"
echo "============================================================"
echo

echo "Izvestaj:"
echo "  $REPORT"
echo

echo "Broj nalaza po grupama:"
echo

BUGPRONE_COUNT=$(grep -c '\[bugprone-' "$REPORT" || true)
PERFORMANCE_COUNT=$(grep -c '\[performance-' "$REPORT" || true)
MODERNIZE_COUNT=$(grep -c '\[modernize-' "$REPORT" || true)
READABILITY_COUNT=$(grep -c '\[readability-' "$REPORT" || true)

echo "  bugprone:       $BUGPRONE_COUNT"
echo "  performance:    $PERFORMANCE_COUNT"
echo "  modernize:      $MODERNIZE_COUNT"
echo "  readability:    $READABILITY_COUNT"

echo
echo "Compiler dijagnostika:"
echo

ERROR_COUNT=$(grep -c ' error: ' "$REPORT" || true)
WARNING_COUNT=$(grep -c ' warning: ' "$REPORT" || true)

echo "  error:          $ERROR_COUNT"
echo "  warning:        $WARNING_COUNT"

echo




TOTAL=$((BUGPRONE_COUNT + PERFORMANCE_COUNT + MODERNIZE_COUNT + READABILITY_COUNT))

echo "Ukupan broj Clang-Tidy nalaza:"
echo "  $TOTAL"

echo

echo "============================================================"
echo "Analiza je zavrsena."
echo "============================================================"
echo
