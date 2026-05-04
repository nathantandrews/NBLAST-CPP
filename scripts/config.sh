#!/bin/bash

BANC_MIRRORED_DIR="/scratch/preserve/wayne/FlyWire/Skeletons/banc_mirrored"
FAFB_DIR="/scratch/preserve/wayne/FlyWire/Skeletons/fafb_banc_space/240721"
FLYWIRE_DIR="/home/ntandre1/research/FlyCircuit"
# defaults
export GENERATOR_KNOWN_CLASS_FILE="data/known-classes/KCg-m-pairs-banc-banc.tsv"
export GENERATOR_NUM_ITERS=100
export GENERATOR_OUT="out/matrix.tsv"

export QUERY_INPUT_SET=aba_long_100.tsv
export QUERY_MATRIX=regression-tests/input/smat.fcwb.tsv
export QUERY_OUT=exp/query-out.tsv

map_dataset() {
  local code="$1"

  case "$code" in
  b) echo "$BANC_MIRRORED_DIR" ;;
  f) echo "$FAFB_DIR" ;;
  c) echo "$FLYWIRE_DIR" ;;
  *)
    echo "Invalid dataset code: $code" >&2
    return 1
    ;;
  esac
}

