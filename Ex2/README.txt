As diretorias existentes no arquivo sao:
	1- CircuitSolver-Parsolver
		contem a solucao do projeto, ou seja, procurar os caminhos da grid paralelamente
	
	2- CircuitSolver-SeqSolver
		contem a solucao do Exercicio 1 do projeto dado pelo corpo docente

	3- inputs
		contem os inputs de texto standard 

	4- results
		contem os outputs criados pelo script doTest.sh

=====================================================================================================

	Para compilar o projeto basta esta na raiz do arquivo e executar o comando $make, que por sua vez 
vai compilar o ParSolver e o SeqSolver.
	Para correr o ParSolver normalmente e necessario estar na diretoria do mesmo e executar o comando
$./CircuitSolver-ParSolver (argumentos).
	Para executar o doTest.sh existem algumas opcoes:

			1- $make doTestAll (nthreads)
				vai correr todos os testes execepto os de 512. Este corre se na raiz do arquivo.

			2- $make doTest
				vai correr um teste default, que e com tamnho 256 e 4 threads, usado nomeadamente para
				testes. Como o anterior tambem se executa na raiz do arquivo

			3- ./doTest.sh (nthreads) (nome do ficheiro)
				para correr um input repetidamente ate o numero de treads indicado, este por sua vez
				tambem se executa na raiz do arquivo.


=====================================================================================================

Usamos duas maquinas para correr os testes:

		Maquina 1:
			OS: Ubuntu 18.04 LTS 64bits (dual boot)
			CPU: Intel(R) Core(TM) i5-7500 @ 3.40GHz
			Max Turbo Frequency: 3.80GHz

		Maquina 2:
			OS:
			CPU:
			Max Turbo Frequency:

=====================================================================================================
