#!/usr/bin/env bash

set -u
set -o pipefail

# ============================================================
# Putanje
# ============================================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

UNIT_TESTS_DIR="$SCRIPT_DIR"
MAFIJA_DIR="$SCRIPT_DIR/../Mafija"

MAFIJA_BUILD="$SCRIPT_DIR/build-mafija-coverage"
QTEST_BUILD="$SCRIPT_DIR/build-qtest-coverage"

COVERAGE_DIR="$SCRIPT_DIR/coverage-report"

MAFIJA_INFO="$COVERAGE_DIR/mafia-tests.info"
QTEST_INFO="$COVERAGE_DIR/qtests.info"
COMBINED_INFO="$COVERAGE_DIR/coverage-combined.info"
FILTERED_INFO="$COVERAGE_DIR/coverage-filtered.info"

HTML_DIR="$COVERAGE_DIR/html"


# ============================================================
# Pomoćna funkcija za naslove
# ============================================================

section()
{
    echo
    echo "============================================================"
    echo " $1"
    echo "============================================================"
}


# ============================================================
# Provera alata
# ============================================================

section "Provera potrebnih alata"

REQUIRED_TOOLS=(
    cmake
    ctest
    gcc
    g++
    gcov
    lcov
    genhtml
)

MISSING_TOOL=0

for tool in "${REQUIRED_TOOLS[@]}"
do
    if command -v "$tool" >/dev/null 2>&1; then
        echo "[OK] $tool -> $(command -v "$tool")"
    else
        echo "[GRESKA] Nedostaje alat: $tool"
        MISSING_TOOL=1
    fi
done

if [ "$MISSING_TOOL" -ne 0 ]; then
    echo
    echo "Instaliraj nedostajuce alate pre pokretanja skripte."
    echo
    echo "Na Ubuntu sistemu:"
    echo
    echo "    sudo apt update"
    echo "    sudo apt install build-essential cmake lcov"
    echo
    exit 1
fi


# ============================================================
# Provera direktorijuma
# ============================================================

section "Provera projekta"

if [ ! -d "$MAFIJA_DIR" ]; then
    echo "[GRESKA] Mafija projekat nije pronadjen:"
    echo "$MAFIJA_DIR"
    exit 1
fi

if [ ! -f "$MAFIJA_DIR/CMakeLists.txt" ]; then
    echo "[GRESKA] Ne postoji:"
    echo "$MAFIJA_DIR/CMakeLists.txt"
    exit 1
fi

if [ ! -f "$UNIT_TESTS_DIR/CMakeLists.txt" ]; then
    echo "[GRESKA] Ne postoji:"
    echo "$UNIT_TESTS_DIR/CMakeLists.txt"
    exit 1
fi

echo "Mafija:"
echo "  $MAFIJA_DIR"

echo
echo "QTest projekat:"
echo "  $UNIT_TESTS_DIR"


# ============================================================
# Čišćenje starih rezultata
# ============================================================

section "Ciscenje prethodnog coverage build-a"

rm -rf "$MAFIJA_BUILD"
rm -rf "$QTEST_BUILD"
rm -rf "$COVERAGE_DIR"

mkdir -p "$COVERAGE_DIR"

echo "Stari build i coverage rezultati su uklonjeni."


# ============================================================
# Coverage opcije
# ============================================================

COVERAGE_FLAGS="--coverage -O0 -g"


# ============================================================
# 1. Build originalnog Mafija projekta
# ============================================================

section "CMake configure - Mafija / Catch2"

cmake \
    -S "$MAFIJA_DIR" \
    -B "$MAFIJA_BUILD" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_C_COMPILER=gcc \
    -DCMAKE_CXX_COMPILER=g++ \
    -DCMAKE_C_FLAGS="$COVERAGE_FLAGS" \
    -DCMAKE_CXX_FLAGS="$COVERAGE_FLAGS" \
    -DCMAKE_EXE_LINKER_FLAGS="--coverage" \
    -DCMAKE_SHARED_LINKER_FLAGS="--coverage"

if [ $? -ne 0 ]; then
    echo "[GRESKA] CMake configure za Mafija projekat nije uspeo."
    exit 1
fi


# ============================================================
# Build Catch2 testova
# ============================================================

section "Build postojecih Catch2 testova"

cmake --build "$MAFIJA_BUILD" --target tests -j"$(nproc)"

if [ $? -ne 0 ]; then
    echo "[GRESKA] Build Catch2 testova nije uspeo."
    exit 1
fi


# ============================================================
# Reset coverage brojaca
# ============================================================

section "Reset coverage brojaca - Catch2"

lcov \
    --directory "$MAFIJA_BUILD" \
    --zerocounters \
    --rc branch_coverage=1



# ============================================================
# Pokretanje Catch2 testova
# ============================================================

section "Pokretanje postojecih Catch2 testova"

CATCH2_STATUS=0

CATCH2_EXECUTABLE="$MAFIJA_BUILD/tests/tests"

if [ ! -x "$CATCH2_EXECUTABLE" ]; then
    echo "[GRESKA] Catch2 executable nije pronadjen:"
    echo "  $CATCH2_EXECUTABLE"
    exit 1
fi

"$CATCH2_EXECUTABLE"

CATCH2_STATUS=$?

if [ "$CATCH2_STATUS" -eq 0 ]; then
    echo
    echo "[OK] Svi Catch2 testovi su prosli."
else
    echo
    echo "[UPOZORENJE] Jedan ili vise Catch2 testova nije proslo."
    echo "Coverage ce ipak biti generisan."
fi



# ============================================================
# Capture Catch2 coverage
# ============================================================

section "Prikupljanje coverage-a - Catch2"

lcov \
    --capture \
    --directory "$MAFIJA_BUILD" \
    --output-file "$MAFIJA_INFO" \
    --rc branch_coverage=1 \
    --ignore-errors mismatch,inconsistent,count

if [ $? -ne 0 ]; then
    echo "[GRESKA] Nije moguce prikupiti Catch2 coverage."
    exit 1
fi


# ============================================================
# 2. Build našeg QTest projekta
# ============================================================

section "CMake configure - novi QTest testovi"

cmake \
    -S "$UNIT_TESTS_DIR" \
    -B "$QTEST_BUILD" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_C_COMPILER=gcc \
    -DCMAKE_CXX_COMPILER=g++ \
    -DCMAKE_C_FLAGS="$COVERAGE_FLAGS" \
    -DCMAKE_CXX_FLAGS="$COVERAGE_FLAGS" \
    -DCMAKE_EXE_LINKER_FLAGS="--coverage" \
    -DCMAKE_SHARED_LINKER_FLAGS="--coverage"

if [ $? -ne 0 ]; then
    echo "[GRESKA] CMake configure za QTest projekat nije uspeo."
    exit 1
fi


# ============================================================
# Build svih QTest testova
# ============================================================

section "Build novih QTest testova"

cmake --build "$QTEST_BUILD" -j"$(nproc)"

if [ $? -ne 0 ]; then
    echo "[GRESKA] Build QTest testova nije uspeo."
    exit 1
fi


# ============================================================
# Reset coverage brojaca
# ============================================================

section "Reset coverage brojaca - QTest"

lcov \
    --directory "$QTEST_BUILD" \
    --zerocounters \
    --rc branch_coverage=1


# ============================================================
# Pokretanje QTest testova
#
# Ovde NE prekidamo skriptu ako neki test padne.
#
# Imamo namerno padajuce testove koji reprodukuju bugove:
#
#   VotingRound:
#     voting_for_nobody
#
#   ChatServer:
#     PlayerSelection fall-through
#
# I oni su i dalje validni za coverage.
# ============================================================

section "Pokretanje novih QTest testova"

QTEST_STATUS=0

ctest \
    --test-dir "$QTEST_BUILD" \
    --output-on-failure

QTEST_STATUS=$?

if [ "$QTEST_STATUS" -eq 0 ]; then
    echo
    echo "[OK] Svi QTest testovi su prosli."
else
    echo
    echo "[UPOZORENJE] Jedan ili vise QTest testova nije proslo."
    echo "Ovo moze biti ocekivano zbog testova koji reprodukuju bugove."
    echo "Coverage ce ipak biti generisan."
fi


# ============================================================
# Capture QTest coverage
# ============================================================

section "Prikupljanje coverage-a - QTest"

lcov \
    --capture \
    --directory "$QTEST_BUILD" \
    --output-file "$QTEST_INFO" \
    --rc branch_coverage=1 \
    --ignore-errors mismatch,inconsistent,count

if [ $? -ne 0 ]; then
    echo "[GRESKA] Nije moguce prikupiti QTest coverage."
    exit 1
fi

# ============================================================
# Spajanje Catch2 + QTest coverage-a
# ============================================================

section "Spajanje coverage rezultata"

lcov \
    --add-tracefile "$MAFIJA_INFO" \
    --add-tracefile "$QTEST_INFO" \
    --output-file "$COMBINED_INFO" \
    --rc branch_coverage=1 \
    --ignore-errors mismatch,inconsistent,corrupt,count

if [ $? -ne 0 ]; then
    echo "[GRESKA] Spajanje coverage rezultata nije uspelo."
    exit 1
fi

# ============================================================
# Filtriranje coverage rezultata
#
# Iz zajednickog coverage-a uklanjamo:
#   - sistemske biblioteke
#   - Qt
#   - Catch2
#   - generisani CMake/Qt kod
#   - originalne Catch2 testove
#   - nase QTest fajlove
#
# Ostaje samo izvorni kod projekta Mafija.
# ============================================================

section "Filtriranje coverage rezultata"

lcov \
    --remove "$COMBINED_INFO" \
    '/usr/*' \
    '*/_deps/*' \
    '*/CMakeFiles/*' \
    '*_autogen/*' \
    '*/tests/*' \
    "$UNIT_TESTS_DIR/qtest/*" \
    --output-file "$FILTERED_INFO" \
    --rc branch_coverage=1 \
    --ignore-errors mismatch,inconsistent,corrupt,count,unused

if [ $? -ne 0 ]; then
    echo "[GRESKA] Filtriranje coverage-a nije uspelo."
    exit 1
fi


# ============================================================
# Tekstualni summary
# ============================================================

section "Coverage summary"

lcov \
    --summary "$FILTERED_INFO" \
    --rc branch_coverage=1


# ============================================================
# HTML izvestaj
# ============================================================

section "Generisanje HTML coverage izvestaja"

genhtml \
    "$FILTERED_INFO" \
    --output-directory "$HTML_DIR" \
    --branch-coverage \
    --title "Mafija - Catch2 + QTest Coverage"

if [ $? -ne 0 ]; then
    echo "[GRESKA] HTML izvestaj nije generisan."
    exit 1
fi


# ============================================================
# Zavrsni izvestaj
# ============================================================

section "Rezultat"

echo "Catch2 test status:"
if [ "$CATCH2_STATUS" -eq 0 ]; then
    echo "  PASS"
else
    echo "  FAIL"
fi

echo

echo "QTest status:"
if [ "$QTEST_STATUS" -eq 0 ]; then
    echo "  PASS"
else
    echo "  FAIL - moguce ocekivani bug-reproduction testovi"
fi

echo
echo "LCOV fajl:"
echo "  $FILTERED_INFO"

echo
echo "HTML coverage izvestaj:"
echo "  $HTML_DIR/index.html"

echo
echo "Za otvaranje izvestaja:"
echo
echo "  xdg-open \"$HTML_DIR/index.html\""
echo

section "Gotovo"

# Namerno vracamo 0 ako je coverage uspesno generisan,
# cak i ako je neki bug-reproduction test pao.
exit 0
