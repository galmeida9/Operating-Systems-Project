#!/bin/bash
n_threads=$1
path=$2

echo '#threads, exec_time, speedup'

start1=$(date +%s.%N)
./CircuitRouter-SeqSolver/CircuitRouter-SeqSolver $path
end1=$(date +%s.%N)
seqTime=$(echo "$end1 - $start1" | bc)
echo 1S, $seqTime, 1

for i in $(seq 1 $n_threads)
do
	start2=$(date +%s.%N)
	./CircuitRouter-ParSolver/CircuitRouter-ParSolver -t $i $path
	end2=$(date +%s.%N)
	parTime=$(echo "$end2 - $start2" | bc)
	speedup=$(echo "$seqTime/$parTime" | bc)
	echo $i, $parTime, $speedup
done
