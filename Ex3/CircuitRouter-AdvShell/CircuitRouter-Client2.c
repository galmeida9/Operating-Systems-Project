#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CLI "/tmp/client2.pipe"
#define BUF 1024

void apanhaCTRLC(int sig){
    unlink(CLI);
    exit(0);
}

int main(int argc, char** argv){
    int fcli, fserv,n;
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
		n = read(0, buffer, BUF-1);
		buffer[n] = '\0';
		if (strcmp(buffer, "exit\n")==0) break;
        write(fserv, path, strlen(path)+1);
		fcli = open(path, O_RDONLY);
		read(fcli, buffer_aux, BUF);
		write(fserv, buffer, strlen(buffer)+1);
		read(fcli, buffer_aux, BUF);
		printf("%s", buffer_aux);
		read(fcli, buffer_aux, BUF);
		printf("%s\n", buffer_aux);
		close(fcli);
    }

    close(fserv);
    close(fcli);
    unlink(CLI);
    exit(0);
}
