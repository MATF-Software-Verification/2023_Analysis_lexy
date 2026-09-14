#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

REPORT_FILE="$SCRIPT_DIR/libfuzzer_report.txt"

rm -f "$REPORT_FILE"

exec > >(tee "$REPORT_FILE") 2>&1

BUILD_DIR="$SCRIPT_DIR/build"
CORPUS_DIR="$SCRIPT_DIR/corpus"
CRASH_DIR="$SCRIPT_DIR/crashes"

echo "=========================================="
echo " LibFuzzer - Mafija protocol"
echo "=========================================="

echo
echo "Provera alata..."
echo

command -v clang >/dev/null 2>&1 || {
    echo "Greska: clang nije instaliran."
    exit 1
}

command -v clang++ >/dev/null 2>&1 || {
    echo "Greska: clang++ nije instaliran."
    exit 1
}

command -v cmake >/dev/null 2>&1 || {
    echo "Greska: cmake nije instaliran."
    exit 1
}

echo "Clang:"
clang --version | head -n 1

echo
echo "Clang++:"
clang++ --version | head -n 1

echo
echo "CMake:"
cmake --version | head -n 1


echo
echo "=========================================="
echo " Priprema pocetnog korpusa"
echo "=========================================="

mkdir -p "$CORPUS_DIR"

if [ -z "$(ls -A "$CORPUS_DIR" 2>/dev/null)" ]; then

    echo "Corpus ne postoji ili je prazan."
    echo "Dodajem pocetne QString i JSON ulaze..."

    # -------------------------
    # QString ulazi
    # -------------------------

    printf '%s' 'message' \
        > "$CORPUS_DIR/qstring_message"

    printf '%s' 'game_state_update' \
        > "$CORPUS_DIR/qstring_game_state_update"

    printf '%s' 'night_started' \
        > "$CORPUS_DIR/qstring_night_started"

    printf '%s' 'night_ended' \
        > "$CORPUS_DIR/qstring_night_ended"

    printf '%s' 'voting_started' \
        > "$CORPUS_DIR/qstring_voting_started"

    printf '%s' 'voting_ended' \
        > "$CORPUS_DIR/qstring_voting_ended"

    printf '%s' 'game_over' \
        > "$CORPUS_DIR/qstring_game_over"

    printf '%s' 'Mafia' \
        > "$CORPUS_DIR/qstring_mafia"

    printf '%s' 'Police' \
        > "$CORPUS_DIR/qstring_police"

    printf '%s' 'Doctor' \
        > "$CORPUS_DIR/qstring_doctor"

    printf '%s' 'Citizen' \
        > "$CORPUS_DIR/qstring_citizen"


    # -------------------------
    # JSON ulazi
    # -------------------------

    printf '%s' \
        '{"type":"message","sender":"Marko","message":"Pozdrav"}' \
        > "$CORPUS_DIR/json_message"

    printf '%s' \
        '{"type":"join_game","username":"Marko"}' \
        > "$CORPUS_DIR/json_join_game"

    printf '%s' \
        '{"type":"leave_game","username":"Marko"}' \
        > "$CORPUS_DIR/json_leave_game"

    printf '%s' \
        '{"type":"night_action","role":"Mafia","target":"Nikola"}' \
        > "$CORPUS_DIR/json_night_action"

    printf '%s' \
        '{"type":"game_state_update","phase":"night"}' \
        > "$CORPUS_DIR/json_game_state_update"

    printf '%s' \
        '{"type":"voting_started"}' \
        > "$CORPUS_DIR/json_voting_started"

    printf '%s' \
        '{"type":"voting_ended","result":"Marko"}' \
        > "$CORPUS_DIR/json_voting_ended"

    printf '%s' \
        '{"type":"game_over","winner":"Mafia"}' \
        > "$CORPUS_DIR/json_game_over"

    # Dodatni validni JSON ulazi za slozenije protocol strukture

    printf '%s' \
    '{"type":"game_state_update","phase":"day","day_number":2,"players":[{"username":"Marko","role":"Mafia","alive":true}, {"username":"Nikola","role":"Doctor","alive":false}]}' \
    > "$CORPUS_DIR/json_game_state_full"

    printf '%s' \
    '{"type":"game_state_update","phase":"night","day_number":3,"players":[{"username":"Ana","role":"Police","alive":true},{"username":"Jovan","role":"Citizen","alive":true}]}' \
    > "$CORPUS_DIR/json_game_state_players"

    printf '%s' \
    '{"type":"voting_ended","votes":{"Marko":"Nikola","Nikola":"Marko","Ana":"Marko"},"target":["Marko"]}' \
    > "$CORPUS_DIR/json_voting_ended_full"

    printf '%s' \
    '{"type":"voting_ended","votes":{"Marko":"Ana","Nikola":"Ana","Ana":"Marko"},"target":["Ana","Marko"]}' \
    > "$CORPUS_DIR/json_voting_ended_tie"

    printf '%s' \
    '{"type":"night_ended","target":"Nikola","info":{"Mafia":"Nikola","Doctor":"Marko","Police":"Ana"}}' \
    > "$CORPUS_DIR/json_night_ended_full"

    printf '%s' \
    '{"type":"role_assignment","username":"Marko","role":"Mafia","all_roles":["Mafia","Police","Doctor","Citizen"]}' \
    > "$CORPUS_DIR/json_role_assignment"

    printf '%s' \
    '{"type":"start_game","players":["Marko","Nikola","Ana","Jovan"],"amount_of_roles":[{"role":"Mafia","role_count":1},{"role":"Police","role_count":1},{"role":"Doctor","role_count":1},{"role":"Citizen","role_count":1}]}' \
    > "$CORPUS_DIR/json_start_game"

    printf '%s' \
    '{"type":"waiting_room_lists","selected":[{"username":"Marko"},{"username":"Ana"}],"unselected":[{"username":"Nikola"},{"username":"Jovan"}]}' \
    > "$CORPUS_DIR/json_waiting_room_lists"

    printf '%s' \
    '{"type":"player_selection","username":"Marko","is_selected":true}' \
    > "$CORPUS_DIR/json_player_selection"

        printf '%s' \
    '{"type":"night_action","role":"Doctor","target":"Marko","username":"Ana"}' \
    > "$CORPUS_DIR/json_night_action_full"

    printf '%s' \
    '{"type":"vote","voter":"Marko","target":"Nikola"}' \
    > "$CORPUS_DIR/json_vote"

    printf '%s' \
    '{"type":"game_over","winner":"Citizens"}' \
    > "$CORPUS_DIR/json_game_over_citizens"


    # -------------------------
    # Nekoliko namerno neispravnih JSON ulaza
    # -------------------------

    printf '%s' \
        '{}' \
        > "$CORPUS_DIR/json_empty_object"

    printf '%s' \
        '{"type":null}' \
        > "$CORPUS_DIR/json_null_type"

    printf '%s' \
        '{"type":123}' \
        > "$CORPUS_DIR/json_number_type"

    printf '%s' \
        '{"type":"message","message":null}' \
        > "$CORPUS_DIR/json_null_message"

    printf '%s' \
        '{"type":"message"' \
        > "$CORPUS_DIR/json_invalid"

    echo "Pocetni corpus je kreiran."

else

    echo "Corpus vec postoji i nije prazan."
    echo "Postojeci corpus se zadrzava."

fi


echo
echo "Broj ulaza u corpus-u:"
find "$CORPUS_DIR" -maxdepth 1 -type f | wc -l


echo
echo "=========================================="
echo " Brisanje prethodnog build direktorijuma"
echo "=========================================="

rm -rf "$BUILD_DIR"

mkdir -p "$BUILD_DIR"
mkdir -p "$CRASH_DIR"


echo
echo "=========================================="
echo " CMake konfiguracija"
echo "=========================================="

CC=clang CXX=clang++ cmake \
    -S "$SCRIPT_DIR" \
    -B "$BUILD_DIR"


echo
echo "=========================================="
echo " Build fuzz targeta"
echo "=========================================="

cmake --build "$BUILD_DIR" -j


echo
echo "=========================================="
echo " Pokretanje libFuzzera"
echo "=========================================="

"$BUILD_DIR/fuzz_protocol" \
    "$CORPUS_DIR" \
    -artifact_prefix="$CRASH_DIR/" \
    -max_len=4096 \
    -max_total_time=60
