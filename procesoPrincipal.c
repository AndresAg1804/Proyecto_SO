#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]) {
    // Crear un pipe
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Crear un proceso hijo
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Proceso hijo
        // Cerrar el extremo de lectura del pipe
        close(pipefd[0]);

        // Redirigir la salida estándar al extremo de escritura del pipe
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        
        char *opcion = argv[1];
        if (strcmp(opcion, "Disco") == 0) {
            if(argc==3){
                char *modo = argv[2];
                if (strcmp(modo, "used") == 0) {  
                    execl("./procesoDisco", "procesoDisco","used", NULL);
                    perror("execl");
                    exit(EXIT_FAILURE);
                }else if (strcmp(modo, "useP") == 0){
                    execl("./procesoDisco", "procesoDisco","useP", NULL);
                    perror("execl");
                    exit(EXIT_FAILURE);
                }else if (strcmp(modo, "mib") == 0){
                    execl("./procesoDisco", "procesoDisco","mib", NULL);
                    perror("execl");
                    exit(EXIT_FAILURE);
                }else if (strcmp(modo, "avail") == 0){
                    execl("./procesoDisco", "procesoDisco","avail", NULL);
                    perror("execl");
                    exit(EXIT_FAILURE);
                }
            }else{
                execl("./procesoDisco", "procesoDisco", NULL);
                perror("execl");
                exit(EXIT_FAILURE);
            }

        }else if (strcmp(opcion, "Memoria") == 0) {
            if(argc==3){
                char *modo = argv[2];
                if (strcmp(modo, "r") == 0) {  
                    execl("./procesoMemoria", "procesoMemoria","r", NULL);
                    perror("execl");
                    exit(EXIT_FAILURE);
                }else{
                    char *pid = argv[2];
                    execl("./procesoMemoria", "procesoMemoria",pid, NULL);
                    perror("execl");
                    exit(EXIT_FAILURE);
                }
            }else{
                execl("./procesoMemoria", "procesoMemoria", NULL);
                perror("execl");
                exit(EXIT_FAILURE);
            }

        }else if (strcmp(opcion, "CPU") == 0) {
            if(argc==2){
                execl("./procesoCPU", "procesoCPU", NULL);
                perror("execl");
                exit(EXIT_FAILURE);
            }else if (argc==3){
                char *pid = argv[2];
                execl("./procesoCPU", "procesoCPU", pid, NULL);
                perror("execl");
                exit(EXIT_FAILURE);
            }

        }else{
            printf("Faltan atributos");
        }
    } else {
        // Proceso padre
        // Cerrar el extremo de escritura del pipe
        close(pipefd[1]);

        // Leer la salida del proceso hijo desde el pipe
        char buffer[4096];
        ssize_t bytes_leidos;
        while ((bytes_leidos = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            
            write(STDOUT_FILENO, buffer, bytes_leidos);
        }

        
        wait(NULL);
    }

    return 0;
}