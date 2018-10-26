#!/bin/bash
n_threads=$1
path=$2
path_res=$(echo "$path".speedups.csv"")

echo '#threads, exec_time, speedup' >> $path_res

start1=$(date +%s.%N)
./CircuitRouter-SeqSolver/CircuitRouter-SeqSolver $path
end1=$(date +%s.%N)
seqTime=$(echo "$end1 - $start1" | bc)
echo 1S, $seqTime, 1 >> $path_res

for i in $(seq 1 $n_threads)
do
	start2=$(date +%s.%N)
	./CircuitRouter-ParSolver/CircuitRouter-ParSolver -t $i $path
	end2=$(date +%s.%N)
	parTime=$(echo "$end2 - $start2" | bc)
	speedup=$(echo "scale=6; ${seqTime}/${parTime}" | bc)
    echo $i, $parTime, ${speedup} >> $path_res
done

cat $path_res
