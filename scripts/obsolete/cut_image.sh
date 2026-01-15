#!/usr/bin/env bash
set -euo pipefail

# cut_image.sh - split an image into 16,777,216-byte (32768*512) blocks
# Usage: cut_image.sh input_image [output_prefix]
# Example: cut_image.sh disk.img disk.img.part-

if [ "$#" -lt 1 ]; then
    echo "Usage: $0 input_file [output_prefix]" >&2
    exit 1
fi

infile="$1"
prefix="${2:-$(basename "$infile").part-}"

if [ ! -f "$infile" ]; then
    echo "Input file not found: $infile" >&2
    exit 2
fi

# block size = 32768 * 512 = 16777216 bytes
blocksize=$((32768 * 512))

# get file size (try GNU stat then BSD stat)
if size=$(stat -c%s -- "$infile" 2>/dev/null); then
    :
elif size=$(stat -f%z -- "$infile" 2>/dev/null); then
    :
else
    echo "Unable to determine file size" >&2
    exit 3
fi

# calculate number of parts and suffix length
parts=$(( (size + blocksize - 1) / blocksize ))
# ensure at least 1
if [ "$parts" -lt 1 ]; then parts=1; fi
# number of digits needed
digits=${#parts}

# use GNU split to produce numeric suffixes (starts at 1)
# output files will be "<prefix>001", "<prefix>002", ...
split --numeric-suffixes=1 --suffix-length="$digits" --bytes="$blocksize" -- "$infile" "$prefix"

echo "Created $parts part(s) with block size $blocksize bytes using prefix '$prefix'"