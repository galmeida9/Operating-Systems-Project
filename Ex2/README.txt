As diretorias existentes no arquivo são:
	1- CircuitSolver-Parsolver
		contém a solução do projeto, ou seja, procurar os caminhos da grid paralelamente
	
	2- CircuitSolver-SeqSolver
		contém a solução do Exercicio 1 do projeto dado pelo corpo docente

	3- inputs
		contém os inputs de texto standard 

	4- results
		contém os outputs criados pelo script doTest.sh

=====================================================================================================

	Para compilar o projeto basta estar na raiz do arquivo e executar o comando $make, que por sua vez 
vai compilar o ParSolver e o SeqSolver.
	Para correr o ParSolver normalmente é necessário estar na diretoria do mesmo e executar o comando
$./CircuitSolver-ParSolver (argumentos).
	Para executar o doTest.sh existem algumas opções (todas executáveis na raiz):

			1- $make doTestAll (nthreads)
				vai correr todos os testes execepto os de 512.

			2- $make doTest
				vai correr um teste default, que resolve um circuito de tamanho 256x256 com 4 threads, usado 
				nomeadamente para testes.

			3- ./doTest.sh (nthreads) (nome do ficheiro)
				para correr um input repetidamente até o número de threads indicado, começando pelo sequencial 


=====================================================================================================

Usamos duas máquinas para correr os testes:

		Máquina 1:
			OS: Ubuntu 18.04 LTS 64bits (dual boot)
			CPU: Intel(R) Core(TM) i5-7500 @ 3.40GHz
			Max Turbo Frequency: 3.80GHz

		Máquina 2:
			OS: OpenSUSE Leap 42.3 64bits
            CPU: Intel(R) Core(TM) i7-7700HQ CPU @ 2.80GHz
			Max Turbo Frequency: 3.80GHz

=====================================================================================================
