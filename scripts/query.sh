#!/bin/bash

. scripts/config.sh

make debug -j8

echo -e "neuron1\tneuron2\tscore" > "$QUERY_OUT"
{
    read -r
    while IFS=$'\t' read -r neuron1 neuron2 score; do
        if [[ "$neuron1" != "NA" && "$neuron2" != "NA" ]]; then
            ./nblast++ -q "$QUERY_MATRIX" -i "$BANC_MIRRORED_DIR,$FAFB_DIR" "$neuron1" "$neuron2"
        fi
    done
} < "$QUERY_TARGET_SET" >> "$QUERY_OUT"