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

void apanhaCTRLC(int sig){
    unlink(CLI);
    exit(0);
}

int main(int argc, char** argv){
    int fcli, fserv;
    char buffer[BUF], path[BUF];
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
		printf("teste\n");
        write(fserv, path, strlen(path));
        write(fserv, "run boas\0", 5);
		getchar();
        /*if ((fcli = open(CLI, O_RDONLY)) <0){
            printf("Erro ao abrir PIPE do cliente1\n");
            exit(-1);
        }
        read(fcli, buffer, BUF);
        printf("%s\n", buffer); */
    }

    close(fserv);
    close(fcli);
    unlink(CLI);
    exit(0);
}
