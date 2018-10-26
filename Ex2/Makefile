MAKEFLAGS = --silent
all:
	mkdir -p results
	echo Making SeqSolver...
	make -C CircuitRouter-SeqSolver/
	echo Making ParSolver...
	make -C CircuitRouter-ParSolver/

clean:	
	echo Cleaning .res, .old and .speedups.csv
	rm -f inputs/*.res inputs/*.old
	rm -f results/*.csv
	make clean -C CircuitRouter-ParSolver/
	make clean -C CircuitRouter-SeqSolver/

doTest:
	./doTest.sh 4 inputs/random-x128-y128-z3-n128.txt

doTestAll:
	./doTestAll.sh 4
