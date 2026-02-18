#!/bin/bash

BANC_MIRRORED_DIR="/scratch/preserve/wayne/FlyWire/Skeletons/banc_mirrored"
FAFB_DIR="/scratch/preserve/wayne/FlyWire/Skeletons/fafb_banc_space/240721"

GENERATOR_KNOWN_CLASS_FILE="data/KCg-m-pairs.tsv"
GENERATOR_NUM_ITERS=100

QUERY_OUT=out/queried-fafb-to-banc-scores.tsv
QUERY_TARGET_SET=data/banc-fafb-scores.tsv
QUERY_MATRIX=out/matrix.tsv