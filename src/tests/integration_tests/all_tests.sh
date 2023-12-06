#!/usr/bin/env bash

declare -a test_files=("test" "tiny" "small" "smedium" "medium" "large")
test_files_dir="/home/lukasbt/projectAD3/data"
test_files_dir="/home/lukasbt/Ugent/3de/semester1/AD3/projectAD3/data"
program="/home/lukasbt/projectAD3/cmake-build-release/projectAD3"
program="/home/lukasbt/Ugent/3de/semester1/AD3/projectAD3/cmake-build-release/projectAD3"


# Loop through each test file and run the tests
for file in "${test_files[@]}"; do
    echo " ======================================= Running tests on file: $file ======================================="

    # Run the compression and extraction tests
    ./test_compress_extract.sh "$program" "$test_files_dir/$file" 3145728

    # Run the test with the tree file
    # ./test_with_tree.sh "$program" "$test_files_dir/$file"
done
