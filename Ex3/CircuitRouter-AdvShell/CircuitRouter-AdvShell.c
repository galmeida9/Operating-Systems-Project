
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
#define BUFFER_SIZE 1024


void childTime(int sig);
int parseArguments(char **argVector, int vectorSize, char *buffer, int bufferSize);
void waitForChild(vector_t *children);
void printChildren(vector_t *children);

vector_t *children;

int main (int argc, char** argv) {

    char *args[MAXARGS + 1];
    char buffer[BUFFER_SIZE];
    char pathPipe[BUFFER_SIZE];
	char msg_serv[] = "Starting SERVER pipe.\n",
		 msg_wait[] = "Wainting for results.\n",
		 msg_recv[] = "Message Received.\n";
    int MAXCHILDREN = -1, fserv, fcli, maxFD, result, n, j;
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
	if ((fserv = open(SERVER_PATH, O_RDONLY | O_NONBLOCK))<0){
		printf("Erro ao inicializar pipe.\n");
		exit(-1);
	}
	
	FD_ZERO(&readset);
	FD_SET(fserv, &readset);
	FD_SET(fileno(stdin), &readset);
	maxFD = fileno(stdin) > fserv ? fileno(stdin) : fserv;

	write(1, msg_wait, strlen(msg_wait));
	signal(SIGCHLD, childTime);
	signal(SIGPIPE, NULL);

    while (1) {
        int numArgs;
		strcpy(pathPipe, "");
		pathPipe[0] = '\0';
		buffer[0] = '\0';

		result = select(maxFD+1, &readset, NULL, NULL, NULL);
		if (result == -1) continue;
		else if (result){
			if (FD_ISSET(fileno(stdin), &readset)){
				fgets(buffer, BUFFER_SIZE, stdin);
			}
			if (FD_ISSET(fserv, &readset)){
				read(fserv, pathPipe, BUFFER_SIZE);
				/*	printf("%s\n", pathPipe);*/
				fcli = open(pathPipe, O_WRONLY);
				write(fcli, msg_recv, strlen(msg_recv)+1);
				read(fserv, buffer, BUFFER_SIZE);																	write(fcli, msg_recv, strlen(msg_recv)+1);
				/*printf("%s\n", buffer);*/
				close(fcli);
			}
		}
		FD_SET(fserv, &readset);
		FD_SET(fileno(stdin), &readset);

        numArgs = parseArguments(args, MAXARGS+1, buffer, BUFFER_SIZE);
		printf("%s\n", buffer);
        /* EOF (end of file) do stdin ou comando "sair" */
        if (numArgs < 0 || (numArgs > 0 && (strcmp(pathPipe, "")==0) && (strcmp(args[0], COMMAND_EXIT) == 0))) {
            printf("CircuitRouter-AdvShell will exit.\n--\n");

            /* Espera pela terminacao de cada filho */
            while (runningChildren > 0) {
                waitForChild(children);
                runningChildren --;
            }

            printChildren(children);
            printf("--\nCircuitRouter-AdvShell ended.\n");
			/*close(fserv);
			unlink(SERVER_PATH);*/
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

/**/

void childTime(int sig){
	pid_t pid;
	int status;
	pid = waitpid(-1, &status, WNOHANG);
	printf("teste\n");
	if (WIFEXITED(status)){
		printf("pid=%d\n", pid);
	}
	signal(SIGCHLD, childTime);
}

int parseArguments(char **argVector, int vectorSize, char *buffer, int bufferSize){
	int numTokens = 0;
  	char *s = " \r\n\t";
	int i;
	char *token;

  	if (argVector == NULL || buffer == NULL || vectorSize <= 0 || bufferSize <= 0)
    	return 0;

  	/* get the first token */
  	token = strtok(buffer, s);

  	/* walk through other tokens */
  	while( numTokens < vectorSize-1 && token != NULL ) {
    	argVector[numTokens] = token;
    	numTokens ++;
    	token = strtok(NULL, s);
  	}

  	for (i = numTokens; i<vectorSize; i++) {
    	argVector[i] = NULL;
  	}

  	return numTokens;
}

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

