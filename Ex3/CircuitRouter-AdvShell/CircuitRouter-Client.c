#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "CircuitRouter-AdvShell.h"

#define CLI "/tmp/client"

int fcli, fserv;
char path[BUFFER_SIZE];

void apanhaINT(int sig){
	close(fserv);
	close(fcli);
	unlink(path);
	exit(0);
}

void leComando(char* ptr, int size){
	char c;
	int i=0, j;
	for (j=0; j<size; j++) ptr[j] = '\0';
	while ((c=getchar())!='\n' && i<=(size-1)){
		ptr[i++] = c;
	}
	ptr[i++] = '\n';
	ptr[i] = '\0';
}

int main(int argc, char** argv){
	pid_t pid;
	char buffer[BUFFER_SIZE], buffer_aux[BUFFER_SIZE], pidNumber[24], *extention = ".pipe";
	strcpy(path, CLI);
	pid = getpid();
	sprintf(pidNumber, "%d", pid);
	strcat(path, pidNumber);
	strcat(path, extention);
	
	signal(SIGINT, apanhaINT);
	signal(SIGPIPE, apanhaINT);

	if (argc!=2){
		printf("Falta o argumento da pipe do servidor\n");
		exit(-1);
	}

	if ( (fserv = open(argv[1], O_WRONLY))<0){
		printf("Erro ao abrir pipe\n");
		exit(-1);
	}
	
	unlink(path);
	
	if (mkfifo(path, 0777) < 0){
		printf("Erro ao iniciar PIPE do cliente1\n");
		exit(-1);		 
	}

	while (1){
		msg_protocol msg;

		buffer_aux[0] = '\0';
		leComando(buffer, BUFFER_SIZE);
		if (strcmp(buffer, "leave\n")==0) break;

		strcpy(msg.pipe, path);
		strcpy(msg.command, buffer);

		write(fserv, &msg, sizeof(msg_protocol));
		if ((fcli = open(path, O_RDONLY))<0) exit(EXIT_FAILURE);
		if ((read(fcli, buffer_aux, BUFFER_SIZE))<0) exit(EXIT_FAILURE);
		write(1, buffer_aux, strlen(buffer_aux));
		close(fcli);
	}

	close(fserv);
	close(fcli);
	unlink(path);
	exit(0);
}
