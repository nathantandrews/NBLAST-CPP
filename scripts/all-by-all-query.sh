#!/bin/bash

. scripts/config.sh
process_args "$@"

make debug

mkdir -p out

mapfile -t ids < <(awk '$1!="NA" && $1!="" {print $1}' "$QUERY_INPUT_SET")

n=${#ids[@]}
echo -e "neuron1\tneuron2\tscore" > "$QUERY_OUT"

{
    for ((i=0; i<n; i++)); do
        for ((j=i+1; j<n; j++)); do
            echo -e "${ids[i]}\t${ids[j]}"
        done
    done
} | ./nblast++ -q "$QUERY_MATRIX" \
-i "$QUERY_DATASET,$TARGET_DATASET" \
 >> "$QUERY_OUT"