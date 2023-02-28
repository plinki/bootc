#!/bin/bash

input="$1"
bin_arg=false

if [[ "$2" == "-b" || "$2" == "--bin" ]]; then
    output="${input%.*}.bin"
    xxd -r -p "$input" "$output"
    echo "To bin"
else
    output="${input%.*}.hex"
    xxd -p "$input" > "$output"
fi
