#!/bin/bash
n_threads=$1
mkdir -p results
for f in $(ls inputs/*.txt | grep -v "512")
do
    input=$(echo "inputs/"$f)
    echo $f
    ./doTest.sh $n_threads $f
done
