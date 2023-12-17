#!/bin/bash

rm source/generated/*

for filename in resources/*; do
    OUTPUT_FILENAME=$(printf "%s.c" "$(basename $filename | sed 's/[^a-zA-Z0-9]/_/g')")
    tcc -run file2c.c "${filename}" > "source/generated/${OUTPUT_FILENAME}"
    echo "${filename} -> source/generated/${OUTPUT_FILENAME}"
done
