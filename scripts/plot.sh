#!/bin/bash

. .venv/bin/activate

python3 plot/scatter.py \
    data/banc-fafb-scores.tsv \
    exp/queried-fafb-to-banc-scores.tsv \
    --label1 NBLAST \
    --label2 NBLAST++ \
    --out exp/scatter-comparison.png \
    > exp/scatter-stats.txt

python3 plot/histogram.py \
    data/banc-fafb-scores.tsv \
    exp/queried-fafb-to-banc-scores.tsv \
    --label1 NBLAST \
    --label2 NBLAST++ \
    --bins 75 \
    --out exp/histogram-comparison.png \
    > exp/histogram-stats.txt

