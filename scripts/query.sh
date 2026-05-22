#!/bin/bash

. scripts/config.sh

make release

process_args "$@"

mkdir out

echo -e "neuron1\tneuron2\tscore" > "$QUERY_OUT"

awk '$1 != "NA" && $2 != "NA" { print $1, $2 }' "$QUERY_INPUT_SET" | \
./nblast++ -q "$QUERY_MATRIX" -i "$QUERY_DATASET,$TARGET_DATASET" \
>> "$QUERY_OUT"