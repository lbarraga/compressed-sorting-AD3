#!/bin/bash

program=$1
original_file=$2
m=$3

# Ensure m is treated as an integer
m=$(($m+0))

# Calculate memory limits
program_memory=$m
memory_limit=$((m * 3 / 2)) # 1.5 times the program memory

compressed_file="${original_file}_compressed"
extracted_file="${original_file}_extracted"
sorted_file="${original_file}_sorted"
sorted_file_again="${original_file}_sorted_again"
sorted_file_reference="${original_file}_sorted_reference"

##################################### TESTS #####################################

# ----------------------------- compress & extract -----------------------------
# Compress and extract the file
systemd-run --scope -p "MemoryMax=${memory_limit}" --user "$program" compress -i "$original_file.txt" -o "$compressed_file.txt" -m "$program_memory" > /dev/null
systemd-run --scope -p "MemoryMax=${memory_limit}" --user "$program" extract -i "$compressed_file.txt" -o "$extracted_file.txt" -m "$program_memory" > /dev/null

# Compare the original and extracted file
if cmp -s "$original_file.txt" "$extracted_file.txt"; then
    echo -e "\t\033[0;32m[PASSED]\033[0m Compress and Extract"
else
    echo -e "\t\033[0;31m[FAILED]\033[0m Compress and Extract"
fi

# ----------------------------- sort & extract -----------------------------
# Sort the compressed file
systemd-run --scope -p "MemoryMax=${memory_limit}" --user "$program" sort -i "$compressed_file.txt" -o "$sorted_file.txt" -m "$program_memory" > /dev/null
systemd-run --scope -p "MemoryMax=${memory_limit}" --user "$program" extract -i "$sorted_file.txt" -o "$extracted_file.txt" -m "$program_memory" > /dev/null

sort "$original_file.txt" -o "${sorted_file_reference}.txt"

# Compare the sorted and extracted file
if cmp -s "$extracted_file.txt" "${sorted_file_reference}.txt"; then
    echo -e "\t\033[0;32m[PASSED]\033[0m Sort & Extract"
else
    echo -e "\t\033[0;31m[FAILED]\033[0m Sort & Extract"
fi

# ----------------------------- compress & extract after sort -----------------------------
# compress the sorted and extracted file
systemd-run --scope -p "MemoryMax=${memory_limit}" --user "$program" compress -i "$extracted_file.txt" -o "$compressed_file.txt" -m "$program_memory" > /dev/null
systemd-run --scope -p "MemoryMax=${memory_limit}" --user "$program" extract -i "$compressed_file.txt" -o "$extracted_file.txt" -m "$program_memory" > /dev/null

# Compare the sorted and extracted file
if cmp -s "$extracted_file.txt" "${sorted_file_reference}.txt"; then
    echo -e "\t\033[0;32m[PASSED]\033[0m Compress & Extract after sort"
else
    echo -e "\t\033[0;31m[FAILED]\033[0m Compress & Extract after sort"
fi

# ----------------------------- sort again and extract back -----------------------------
systemd-run --scope -p "MemoryMax=${memory_limit}" --user "$program" sort -i "$sorted_file.txt" -o "$sorted_file_again.txt" -m "$program_memory" > /dev/null
systemd-run --scope -p "MemoryMax=${memory_limit}" --user "$program" extract -i "$sorted_file_again.txt" -o "$extracted_file.txt" -m "$program_memory" > /dev/null

# Compare the sorted and extracted file
if cmp -s "$extracted_file.txt" "${sorted_file_reference}.txt"; then
    echo -e "\t\033[0;32m[PASSED]\033[0m Sort again and extract back"
else
    echo -e "\t\033[0;31m[FAILED]\033[0m Sort again and extract back"
fi
