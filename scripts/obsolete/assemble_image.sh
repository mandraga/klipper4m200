#!/bin/bash

# Reassembles an image taken in parts using a script shell from linux injecting commands to u-boot shell.
set -euo pipefail

BASE_DIR="/home/patrick/moredata/emmc"
PART_PREFIX="emmc.img"
OUT="emmc.img"

TMP="$(mktemp "${OUT}.tmp.XXXXXX")"
cleanup() { rm -f "$TMP"; }
trap cleanup EXIT

for i in {0..378}; do
    PART="${BASE_DIR}/${PART_PREFIX}${i}"
    if [[ ! -f "$PART" ]]; then
        echo "Missing part: $PART" >&2
        exit 1
    fi
    cat "$PART" >> "$TMP"
done

mv -v "$TMP" "${BASE_DIR}/${OUT}"
trap - EXIT
echo "Assembled image: $OUT"
