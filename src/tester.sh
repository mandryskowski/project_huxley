#!/bin/bash

source_directory="../armv8_testsuite/test/test_cases/"
target_directory="../armv8_testsuite/test/expected_results/"

find "$source_directory" -type f -name "*.s" | while read -r source_file; do
    if [[ -f "$source_file" ]]; then
        # Extract the filename without extension
        source_filename=$(basename "$source_file")
        source_filename_noext="${source_filename%.*}"
        relative_path=$(echo "$source_file" | sed "s|$source_directory||")
        relative_directory=$(dirname "$relative_path")

        target_filename="$source_filename_noext""_exp.bin"

        target_file="$target_directory$relative_directory/$target_filename"  # Construct the target file path


        if [[ -f "$target_file" ]]; then
            # Perform the comparison here
            ./assemble "$source_file" out.out
            if diff out.out "$target_file"
            then
                echo -ne "\033[3;32mTEST $source_filename OK\033[0m\r"
            else
                echo -ne "\033[3;31mTEST $source_filename WRONG\033[0m\n"
                break
            fi
        else
            echo "No corresponding file found for $source_file"
            exit 1
        fi
    fi
done