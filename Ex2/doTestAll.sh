#!/bin/bash
n_threads=$1
for f in $(ls inputs/*.txt | grep -v "512")
do
    input=$(echo "inputs/"$f)
    echo $f
    ./doTest.sh $n_threads $f
done
