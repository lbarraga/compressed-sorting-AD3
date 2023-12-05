#!/bin/bash

program=$1
original_file=$2
m=$3

compressed_file="${original_file}_compressed"
extracted_file="${original_file}_extracted"
sorted_file="${original_file}_sorted"
sorted_file_reference="${original_file}_sorted_reference"

##################################### TESTS #####################################

# ----------------------------- compress & extract -----------------------------
# Compress and extract the file
$program compress -i "$original_file.txt" -o "$compressed_file.txt" -m "$m" > /dev/null
$program extract -i "$compressed_file.txt" -o "$extracted_file.txt" -m "$m" > /dev/null

# Compare the original and extracted file
if cmp -s "$original_file.txt" "$extracted_file.txt"; then
    echo -e "\033[0;32m[PASSED]\033[0m"
else
    echo -e "\033[0;31mCompress and extract test of $original_file.txt with m = $m: FAILED\033[0m"
fi

# ----------------------------- sort & extract -----------------------------
# Sort the compressed file
$program sort -i "$compressed_file.txt" -o "$sorted_file.txt" -m "$m" > /dev/null
$program extract -i "$sorted_file.txt" -o "$extracted_file.txt" -m "$m" > /dev/null

sort "$original_file.txt" -o "${sorted_file_reference}.txt"

# Compare the sorted and extracted file
if cmp -s "$extracted_file.txt" "${sorted_file_reference}.txt"; then
    echo -e "\033[0;32m[PASSED]\033[0m"
else
    echo -e "\033[0;31mSort test of $original_file.txt with m = $m: FAILED\033[0m"
fi

