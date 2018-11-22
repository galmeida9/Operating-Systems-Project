
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

void readFromStdin(void* buffer){
    char buffer_aux[BUFFER_SIZE];
    char* buffer_stdin = (char*) buffer;
    strcpy(buffer_stdin, "");
    while (strcmp(buffer, "exit")){
        fgets(buffer_aux, BUFFER_SIZE, stdin);
        buffer_stdin = strtok(buffer_aux, "\n");
    }
}

void readFromPipe(void* buffer){
    /*
    char** buffer_aux = (char**) buffer;
    char* newPipe = (char*) buffer_aux[0];
    char* buffer_pipe = (char*) buffer_aux[1];
    strcpy(newPipe, "");
    strcpy(buffer_pipe, "");
    int fserv;
    if ( (fserv = open(SERVER_PATH, O_RDONLY)) < 0){
        printf("Erro ao inicilizar pipe.\n");
        exit(-1);
    }

    while (1){
        read(fserv, newPipe, BUFFER_SIZE);
        read(fserv, buffer_pipe, BUFFER_SIZE);
    }*/
}


int main (int argc, char** argv) {

    char *args[MAXARGS + 1];
    char *buffer;
    char buffer_stdin[BUFFER_SIZE];
    char buffer_pipe[2][BUFFER_SIZE];
    char newPipe[BUFFER_SIZE];
    int MAXCHILDREN = -1;
    vector_t *children;
    int runningChildren = 0;
    pthread_t threads[2];

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
    
    if (pthread_create(&threads[0], NULL, (void*) readFromStdin, (void*) buffer_stdin));
    if (pthread_create(&threads[1], NULL, (void*) readFromPipe, (void*) buffer_pipe));
   
    while (1) {
        int numArgs;
        
        while (strcmp(buffer_stdin, "")==0 && strcmp(buffer_pipe[1], "")==0);
        if (strcmp(buffer_stdin, "")!=0) buffer = buffer_stdin;
        else if (strcmp(buffer_pipe[1], "")!=0) buffer = buffer_pipe[1];

        numArgs = readLineArguments(args, MAXARGS+1, buffer, BUFFER_SIZE);
        printf("%s", buffer);
        /* EOF (end of file) do stdin ou comando "sair" */
        if (numArgs < 0 || (numArgs > 0 && buffer==buffer_stdin && (strcmp(args[0], COMMAND_EXIT) == 0))) {
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
