#!/bin/bash

BANC_MIRRORED_DIR="/scratch/preserve/wayne/FlyWire/Skeletons/banc_mirrored"
FAFB_DIR="/scratch/preserve/wayne/FlyWire/Skeletons/fafb_banc_space/240721"
# defaults
QUERY_DATASET="$BANC_MIRRORED_DIR"
TARGET_DATASET="$FAFB_DIR"
QUERY_DATASET=/home/ntandre1/research/NBLAST-CPP/data/costa-2016/fctraces20-swc
TARGET_DATASET=/home/ntandre1/research/NBLAST-CPP/data/costa-2016/fctraces20-swc


GENERATOR_KNOWN_CLASS_FILE="data/known-classes/KCg-m-pairs-banc-banc.tsv"
GENERATOR_NUM_ITERS=100
GENERATOR_OUT="out/matrix.tsv"

QUERY_INPUT_SET=data/costa-2016/fctraces20-nblast_scores.tsv
QUERY_MATRIX=data/costa-2016/smat.fcwb.tsv
QUERY_OUT=exp/query-out.tsv

map_dataset() {
    local code="$1"

    case "$code" in
        b) echo "$BANC_MIRRORED_DIR" ;;
        f) echo "$FAFB_DIR" ;;
        *)
            echo "Invalid dataset code: $code" >&2
            return 1
            ;;
    esac
}
process_args() {
    while [[ "$#" -gt 0 ]]; do
        case $1 in
            -d)
                IFS=',' read -r DATASET_INDICATOR_QUERY DATASET_INDICATOR_TARGET <<< "$2"
                QUERY_DATASET=$(map_dataset $DATASET_INDICATOR_QUERY)
                TARGET_DATASET=$(map_dataset $DATASET_INDICATOR_TARGET)
                shift
                ;;
            -o)
                INPUT=$1
                shift
                ;;
            *)
                echo "Unknown option: $1"
                exit 1
                ;;
        esac
        shift
    done
}