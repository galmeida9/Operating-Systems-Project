#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CLI "/tmp/client"
#define BUF 1024

int fcli, fserv;
char path[BUF];

void apanhaCTRLC(int sig){
	close(fserv);
	close(fcli);
    unlink(path);
    exit(0);
}

int main(int argc, char** argv){
    int pid;
    char buffer[BUF], buffer_aux[BUF], pidNumber[24], *extention = ".pipe";
	strcpy(path, CLI);
    pid = getpid();
    sprintf(pidNumber, "%d", pid);
    strcat(path, pidNumber);
    strcat(path, extention);
    
    if (argc!=2){
        printf("Falta o argumento da pipe do servidor\n");
        exit(-1);
    }
    
    signal(SIGINT, apanhaCTRLC);

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
		/*n = read(0, buffer, BUF-1);
		buffer[n] = '\0';*/
		fgets(buffer, BUF-2, stdin);
		buffer[BUF-2] = '\n';
		buffer[BUF-1] = '\0';

		if (strcmp(buffer, "leave\n")==0) break;
        write(fserv, path, strlen(path)+1);
		fcli = open(path, O_RDONLY);
		read(fcli, buffer_aux, BUF);
		write(fserv, buffer, strlen(buffer)+1);
		read(fcli, buffer_aux, BUF);
		read(fcli, buffer_aux, BUF);
		write(1, buffer_aux, strlen(buffer_aux));
		close(fcli);
    }

    close(fserv);
    close(fcli);
    unlink(CLI);
    exit(0);
}
