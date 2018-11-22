
/*
// Projeto SO - exercise 3, version 1
// Sistemas Operativos, DEI/IST/ULisboa 2018-19
*/

#include "lib/commandlinereader.h"
#include "lib/vector.h"
#include "CircuitRouter-AdvShell.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>

#define COMMAND_EXIT "exit"
#define COMMAND_RUN "run"
#define SERVER_PATH "/tmp/servidor.pipe"

#define MAXARGS 3
#define BUFFER_SIZE 100

void waitForChild(vector_t *children) {
    while (1) {
        child_t *child = malloc(sizeof(child_t));
        if (child == NULL) {
            perror("Error allocating memory");
            exit(EXIT_FAILURE);
        }
        child->pid = wait(&(child->status));
        if (child->pid < 0) {
            if (errno == EINTR) {
                /* Este codigo de erro significa que chegou signal que interrompeu a espera
                   pela terminacao de filho; logo voltamos a esperar */
                free(child);
                continue;
            } else {
                perror("Unexpected error while waiting for child.");
                exit (EXIT_FAILURE);
            }
        }
        vector_pushBack(children, child);
        return;
    }
}

void printChildren(vector_t *children) {
    for (int i = 0; i < vector_getSize(children); ++i) {
        child_t *child = vector_at(children, i);
        int status = child->status;
        pid_t pid = child->pid;
        if (pid != -1) {
            const char* ret = "NOK";
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                ret = "OK";
            }
            printf("CHILD EXITED: (PID=%d; return %s)\n", pid, ret);
        }
    }
    puts("END.");
}


int main (int argc, char** argv) {

    char *args[MAXARGS + 1];
    char buffer[BUFFER_SIZE];
    char pathPipe[BUFFER_SIZE];
	char msg_serv[] = "Starting SERVER pipe.\n",
		 msg_wait[] = "Wainting for results.\n";
    int MAXCHILDREN = -1, fserv, fcli, maxFD, result, n;
    vector_t *children;
    int runningChildren = 0;
	fd_set readset;

    if(argv[1] != NULL){
        MAXCHILDREN = atoi(argv[1]);
    }

    children = vector_alloc(MAXCHILDREN);

    unlink(SERVER_PATH);

    if (mkfifo(SERVER_PATH, 0777)<0){
        printf("Erro ao criar pipe.\n");
        exit(-1);
    }

    printf("Welcome to CircuitRouter-AdvShell\n\n");
	
	write(1, msg_serv, strlen(msg_wait));
	if ((fserv = open(SERVER_PATH, O_RDONLY))<0){
		printf("Erro ao inicializar pipe.\n");
		exit(-1);
	}
	
	FD_ZERO(&readset);
	FD_SET(fserv, &readset);
	FD_SET(fileno(stdin), &readset);
	maxFD = fileno(stdin) > fserv ? fileno(stdin) : fserv;

	write(1, msg_wait, strlen(msg_wait));
    
    while (1) {
        int numArgs;
		strcpy(pathPipe, "");

		result = select(maxFD+1, &readset, NULL, NULL, NULL);
		if (result == -1) perror("select()");
		else if (result){
			if (FD_ISSET(fileno(stdin), &readset)){
				n = read(0, buffer, BUFFER_SIZE);
			}
			if (FD_ISSET(fserv, &readset)){
				printf("Oie\n");
				read(fserv, pathPipe, BUFFER_SIZE);
				printf("%s\n", pathPipe);
				printf("wut\n");
				read(fserv, buffer, BUFFER_SIZE);
				printf("%s\n", buffer);
			}
		}
		continue;
        
        numArgs = readLineArguments(args, MAXARGS+1, buffer, BUFFER_SIZE);
        printf("%s", buffer);
        /* EOF (end of file) do stdin ou comando "sair" */
        if (numArgs < 0 || (numArgs > 0 && strcmp(pathPipe, "") && (strcmp(args[0], COMMAND_EXIT) == 0))) {
            printf("CircuitRouter-AdvShell will exit.\n--\n");

            /* Espera pela terminacao de cada filho */
            while (runningChildren > 0) {
                waitForChild(children);
                runningChildren --;
            }

            printChildren(children);
            printf("--\nCircuitRouter-AdvShell ended.\n");
            break;
        }

        else if (numArgs > 0 && strcmp(args[0], COMMAND_RUN) == 0){
            int pid;
            if (numArgs < 2) {
                printf("%s: invalid syntax. Try again.\n", COMMAND_RUN);
                continue;
            }
            if (MAXCHILDREN != -1 && runningChildren >= MAXCHILDREN) {
                waitForChild(children);
                runningChildren--;
            }

            pid = fork();
            if (pid < 0) {
                perror("Failed to create new process.");
                exit(EXIT_FAILURE);
            }

            if (pid > 0) {
                runningChildren++;
                printf("%s: background child started with PID %d.\n\n", COMMAND_RUN, pid);
                continue;
            } else {
                char seqsolver[] = "../CircuitRouter-SeqSolver/CircuitRouter-SeqSolver";
                char *newArgs[3] = {seqsolver, args[1], NULL};

                execv(seqsolver, newArgs);
                perror("Error while executing child process"); // Nao deveria chegar aqui
                exit(EXIT_FAILURE);
            }
        }

        else if (numArgs == 0){
            /* Nenhum argumento; ignora e volta a pedir */
            continue;
        }
        else
            printf("Unknown command. Try again.\n");

    }

    for (int i = 0; i < vector_getSize(children); i++) {
        free(vector_at(children, i));
    }
    vector_free(children);

    return EXIT_SUCCESS;
}
