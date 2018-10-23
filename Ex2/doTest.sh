#!/bin/bash
n_threads=$1
path=$2

echo '#threads, exec_time, speedup'

start1=$(date +%s)
./CircuitRouter-SeqSolver/CircuitRouter-SeqSolver $path
end1=$(date +%s)
runtime=$(echo "$end1 - $start1" |bc)
echo 1S, $runtime, 1

for i in $(seq 1 $n_threads)
do
	./CircuitRouter-ParSolver/CircuitRouter-ParSolver -t $i $path
	echo $i, 0, 0
done
