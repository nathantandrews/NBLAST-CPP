#!/bin/bash

QUERY_DIR=b
TARGET_DIR=f

QUERY_OUT=fafb-to-banc.txt
QUERY_ERR=fafb-to-banc-err.txt

echo "neuron1 neuron2 score" >> "fafb-to-banc.txt"

{
    read -r
    while IFS=$'\t' read -r cols neuron1 neuron2 score; do
        QUERY="$neuron1"
        TARGET="$neuron2"
        make query -j8 \
            QD="$QUERY_DIR" \
            TD="$TARGET_DIR" \
            QUERY_OUT="$QUERY_OUT" \
            QUERY_ERR="$QUERY_ERR" \
            QUERY="$QUERY" \
            TARGET="$TARGET"
    done 
} < banc-fafb-simple.tsv