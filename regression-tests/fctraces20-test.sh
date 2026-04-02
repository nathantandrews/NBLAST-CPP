#!/bin/bash

make debug

mkdir -p out

QUERY_OUT=out/fctraces20-test.out

mapfile -t ids < <(awk '$1!="NA" && $1!="" {print $1}' "regression-tests/input/fctraces20-allbyall.tsv")

n=${#ids[@]}
echo -e "neuron1\tneuron2\tscore" > "$QUERY_OUT"

{
    for ((i=0; i<n; i++)); do
        for ((j=i+1; j<n; j++)); do
            echo -e "${ids[i]}\t${ids[j]}"
        done
    done
} | ./nblast++ -q "regression-tests/input/smat.fcwb.tsv" \
-i "regression-tests/input/fctraces20-swc,regression-tests/input/fctraces20-swc" \
 > "$QUERY_OUT"

diff "regression-tests/verify/fctraces20-test.out" "$QUERY_OUT"