#!/bin/bash

. scripts/config.sh

QUERY_DATASET=$(map_dataset "$1")
TARGET_DATASET=$(map_dataset "$2")
QUERY_INPUT_SET="$3"

make clean >/dev/null
make debug >/dev/null

mkdir -p out

time (cat "$QUERY_INPUT_SET" | ./nblast++ -a -q "$QUERY_MATRIX" \
  -i "$QUERY_DATASET,$TARGET_DATASET") 2>query-times.txt
