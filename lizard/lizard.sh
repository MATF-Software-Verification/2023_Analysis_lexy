#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
MAFIJA_DIR="$SCRIPT_DIR/../Mafija"
OUTPUT_FILE="$SCRIPT_DIR/lizard_results.txt"

echo "=========================================="
echo " Lizard analiza - Mafija"
echo "=========================================="

echo
echo "Provera alata..."

if ! command -v lizard >/dev/null 2>&1; then
    echo "Greska: lizard nije instaliran."
    echo "Instalacija:"
    echo "  pipx install lizard"
    exit 1
fi

echo
echo "Lizard verzija:"
lizard --version

echo
echo "=========================================="
echo " Priprema analize"
echo "=========================================="

if [ ! -d "$MAFIJA_DIR" ]; then
    echo "Greska: Mafija direktorijum nije pronadjen:"
    echo "$MAFIJA_DIR"
    exit 1
fi

echo "Mafija direktorijum:"
echo "$MAFIJA_DIR"

echo
echo "Brisanje prethodnog rezultata..."
rm -f "$OUTPUT_FILE"

echo
echo "=========================================="
echo " Pokretanje Lizard analize"
echo "=========================================="
echo

lizard \
    -C 10 \
    -L 50 \
    -x "*/tests/*" \
    "$MAFIJA_DIR" \
    2>&1 | tee "$OUTPUT_FILE"

echo
echo "=========================================="
echo " Analiza zavrsena"
echo "=========================================="
echo
echo "Rezultat je sacuvan u:"
echo "$OUTPUT_FILE"
