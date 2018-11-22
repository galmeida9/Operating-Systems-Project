#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CLI "/tmp/client1.pipe"
#define BUF 1024

void leComando(char* ptr, int size){
	char c;
	int i=0, j;
	for (j=0; j<size; j++) ptr[j] = '\0';
	while ((c=getchar())!='\n' && i<(size+1)){
		ptr[i++] = c;
	}
	ptr[i++] = '\n';
	ptr[i] = '\0';
}

void apanhaCTRLC(int sig){
    unlink(CLI);
    exit(0);
}

int main(int argc, char** argv){
    int fcli, fserv;
    char buffer[BUF], path[BUF], buffer_aux[BUF];
	strcpy(path, CLI);

    if (argc!=2){
        printf("Falta o argumento da pipe do servidor\n");
        exit(-1);
    }
    
    signal(SIGINT, apanhaCTRLC);

    if ( (fserv = open(argv[1], O_WRONLY))<0){
        printf("Erro ao abrir pipe\n");
        exit(-1);
    }
    
    unlink(CLI);
    
    if (mkfifo(CLI, 0777) < 0){
        printf("Erro ao iniciar PIPE do cliente1\n");
        exit(-1);        
    }

    while (1){
		leComando(buffer, BUF);
		/*if (strcmp(buffer, "exit\n")==0) break;*/
        write(fserv, path, strlen(path)+1);
		fcli = open(path, O_RDONLY);
		read(fcli, buffer_aux, BUF);
		printf("1-%s", buffer_aux);
		printf("Buffer: %s", buffer);
		write(fserv, buffer, strlen(buffer)+1);
		read(fcli, buffer_aux, BUF);
		printf("2-%s", buffer_aux);
		close(fcli);
    }

    close(fserv);
    close(fcli);
    unlink(CLI);
    exit(0);
}
