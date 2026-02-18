#!/bin/bash

. scripts/config.sh

make clean
make debug -j8

./nblast++ -g "$GENERATOR_KNOWN_CLASS_FILE","$GENERATOR_NUM_ITERS" -i "$BANC_MIRRORED_DIR","$FAFB_DIR"