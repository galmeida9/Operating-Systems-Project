all:
	make -C CircuitRouter-SeqSolver/
	make -C CircuitRouter-ParSolver/

clean:	
	rm -f inputs/*.res inputs/*.old
	make clean -C CircuitRouter-ParSolver/
	make clean -C CircuitRouter-SeqSolver/

run:
	./doTest.sh 4 inputs/random-x128-y128-z5-n128.txt
