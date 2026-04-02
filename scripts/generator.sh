#!/bin/bash

. scripts/config.sh

make clean
make debug

process_args "$@"

mkdir out

/usr/bin/time -f "real %E user %U sys %S mem %M KB" ./nblast++ -g "$GENERATOR_KNOWN_CLASS_FILE","$GENERATOR_NUM_ITERS" \
-i "$QUERY_DATASET","$TARGET_DATASET" -d  \
> "$GENERATOR_OUT"