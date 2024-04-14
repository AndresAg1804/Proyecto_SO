#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]){
    if(argc==2){
		char *opcion = argv[1];
        if (strcmp(opcion, "r") == 0) {
             FILE *fp = fopen("/proc/meminfo", "r");
            if (fp == NULL) {
            perror("Error al abrir /proc/meminfo");
            exit(EXIT_FAILURE);
            }
            char buffer[256];
            float memoria_total = 0.0f;

            // Leer el archivo línea por línea
            while (fgets(buffer, sizeof(buffer), fp)) {
            // Buscar la línea que contiene "MemTotal"
                if (strstr(buffer, "MemTotal")) {
                    // Extraer el valor numérico de la línea
                    float valorT;
                    sscanf(buffer, "%*s%f", &valorT);
                    // Convertir kilobytes a gigabytes
                    memoria_total = valorT ;
                    break;
                }
            }
            fclose(fp);
            
            FILE *fpl = fopen("/proc/meminfo", "r");
            if (fpl == NULL) {
            perror("Error al abrir /proc/meminfo");
            exit(EXIT_FAILURE);
            }
            char bufferl[256];
            float memoria_libre = 0.0f;

            // Leer el archivo línea por línea
            while (fgets(bufferl, sizeof(bufferl), fpl)) {
            // Buscar la línea que contiene "MemTotal"
                if (strstr(bufferl, "MemAvailable") || strstr(bufferl, "MemFree")) {
                    // Extraer el valor numérico de la línea
                    float valorL;
                    sscanf(bufferl, "%*s%f", &valorL);
                    // Convertir kilobytes a gigabytes
                    memoria_libre = valorL;
                    break;
                }
            }
            fclose(fpl);

            float porcentaje_utilizado = ((memoria_total - memoria_libre) / memoria_total) * 100.0f;
            printf("Porcentaje de memoria real utilizada: %.2f%%\n",porcentaje_utilizado);



        }else{
            char *pid = argv[1];
            char path[256];
            sprintf(path, "/proc/%s/status", pid);
            
            char buffer3[256];
            float vmSize = 0.0f;
            FILE *fp3 = fopen(path, "r");
            if (fp3 == NULL) {
                perror("Error al abrir el archivo status del proceso");
                exit(EXIT_FAILURE);
            }
            // Leer el archivo línea por línea
            while (fgets(buffer3, sizeof(buffer3), fp3)) {
                // Buscar la línea que contiene "VmRSS"
                if (strstr(buffer3, "VmSize")) {
                    // Extraer el valor numérico de la línea
                    float valor2;
                    sscanf(buffer3, "%*s%f", &valor2);
                    
                    vmSize = valor2;
                    break;
                }

            }
            fclose(fp3);

             FILE *fpV = fopen("/proc/meminfo", "r");
            if (fpV == NULL) {
            perror("Error al abrir /proc/meminfo");
            exit(EXIT_FAILURE);
            }

            char bufferV[256];
            float memoriaV_total = 0.0f;

            // Leer el archivo línea por línea
            while (fgets(bufferV, sizeof(bufferV), fpV)) {
            // Buscar la línea que contiene "MemTotal"
            if (strstr(bufferV, "SwapTotal")) {
                // Extraer el valor numérico de la línea
                float valorV;
                sscanf(bufferV, "%*s%f", &valorV);
                // Convertir kilobytes a gigabytes
                memoriaV_total = valorV;
                break;
            }
        }

            fclose(fpV);

            float porcentajeV=vmSize*100/memoriaV_total;
            printf("Porcentaje de memoria Virtual utilizada para el proceso : %.2f%%\n",porcentajeV);
        }
    }
    else{

    //////////////////////////////////////////////////////////////Hago una lista con todos los PID de los procesos///////////////////////////////////////////////////////////////
        // Abrir un flujo para leer la salida del comando
        FILE *fp1 = popen("ls /proc | grep '^[0-9]'", "r");
        if (fp1 == NULL) {
            perror("popen");
            return 1;
        }

        // Declarar un vector para almacenar los PID
        char pid_list[1000][10];
        int num_pids = 0;

        // Leer la salida del comando línea por línea
        char line[10];
        while (fgets(line, sizeof(line), fp1) != NULL && num_pids < 1000) {
            // Eliminar el salto de línea al final de la línea
            line[strcspn(line, "\n")] = '\0';
            // Copiar el PID al vector
            strcpy(pid_list[num_pids], line);
            num_pids++;
        }
        
        // Cerrar el flujo
        pclose(fp1);

        
    ////////////////////////////////////////////////////////////////////////////Empiezo a recorrer la lista de PID //////////////////////////////////////////////////////////////////////////////////////////////////

        for (int i = 0; i < num_pids; i++) {
            char* pid = pid_list[i];
            char path[256];
            sprintf(path, "/proc/%s/status", pid);

            // Abrir el archivo correspondiente al proceso
        

    ////////////////////////////////////////////////////////Busco el nombre del proceso//////////////////////////////////////////////////////////////////////////////////////////
            FILE *fp2 = fopen(path, "r");
            if (fp2 == NULL) {
                perror("Error al abrir el archivo status del proceso");
                exit(EXIT_FAILURE);
            }

            char buffer2[256];
            char name[256];
            while (fgets(buffer2, sizeof(buffer2), fp2)) {
                if (strstr(buffer2, "Name")) {
                        
                        sscanf(buffer2, "%*s%s", name);
                        
                        
                        
                        break;
                    }
            }
            
            fclose(fp2);

    //////////////////////////////////////Busco la cantidad de uso de memoria real/////////////////////////////////////////////////////////////////////////////////////  
            FILE *fp = fopen(path, "r");
            if (fp == NULL) {
                perror("Error al abrir el archivo status del proceso");
                exit(EXIT_FAILURE);
            }
            char buffer1[256];
            float vmrss = 0.0f;
            
            // Leer el archivo línea por línea
            while (fgets(buffer1, sizeof(buffer1), fp)) {
                // Buscar la línea que contiene "VmRSS"
                if (strstr(buffer1, "VmRSS")) {
                    // Extraer el valor numérico de la línea
                    float valor;
                    sscanf(buffer1, "%*s%f", &valor);
                    
                    vmrss = valor;
                    break;
                }

            }
            fclose(fp);
    //////////////////////////////////////Busco la cantidad de uso de memoria Virtual/////////////////////////////////////////////////////////////////////////////////////  
            char buffer3[256];
            float vmSize = 0.0f;
            FILE *fp3 = fopen(path, "r");
            if (fp3 == NULL) {
                perror("Error al abrir el archivo status del proceso");
                exit(EXIT_FAILURE);
            }
            // Leer el archivo línea por línea
            while (fgets(buffer3, sizeof(buffer3), fp3)) {
                // Buscar la línea que contiene "VmRSS"
                if (strstr(buffer3, "VmSize")) {
                    // Extraer el valor numérico de la línea
                    float valor2;
                    sscanf(buffer3, "%*s%f", &valor2);
                    
                    vmSize = valor2;
                    break;
                }

            }
            fclose(fp3);
    /////////////////////////////////////////////////////////////////////Busco la cantidad de memoria Total//////////////////////////////////////////////////////////////////////////////////////////////////////

            FILE *fpT = fopen("/proc/meminfo", "r");
            if (fpT == NULL) {
            perror("Error al abrir /proc/meminfo");
            exit(EXIT_FAILURE);
            }

            char buffer[256];
            float memoria_total = 0.0f;

            // Leer el archivo línea por línea
            while (fgets(buffer, sizeof(buffer), fpT)) {
            // Buscar la línea que contiene "MemTotal"
            if (strstr(buffer, "MemTotal")) {
                // Extraer el valor numérico de la línea
                float valorT;
                sscanf(buffer, "%*s%f", &valorT);
                // Convertir kilobytes a gigabytes
                memoria_total = valorT ;
                break;
            }
        }

            fclose(fpT);
    /////////////////////////////////////////////////////////////////////Busco la cantidad de memoria virtual Total//////////////////////////////////////////////////////////////////////////////////////////////////////

            FILE *fpV = fopen("/proc/meminfo", "r");
            if (fpV == NULL) {
            perror("Error al abrir /proc/meminfo");
            exit(EXIT_FAILURE);
            }

            char bufferV[256];
            float memoriaV_total = 0.0f;

            // Leer el archivo línea por línea
            while (fgets(bufferV, sizeof(bufferV), fpV)) {
            // Buscar la línea que contiene "MemTotal"
            if (strstr(bufferV, "SwapTotal")) {
                // Extraer el valor numérico de la línea
                float valorV;
                sscanf(bufferV, "%*s%f", &valorV);
                // Convertir kilobytes a gigabytes
                memoriaV_total = valorV;
                break;
            }
        }

            fclose(fpV);
        
        ///////////////////////////////////////////////////////////////////////////////Imprimo los resultados////////////////////////////////////////////////////////////////////
            float porcentaje=vmrss*100/memoria_total;
            float porcentajeV=vmSize*100/memoriaV_total;
        
        printf("PID del proceso: %s\n", pid);
        printf("Nombre del proceso: %s\n", name);
        printf("Porcentaje de memoria real utilizada para el proceso : %.2f%%\n",porcentaje);
        printf("Porcentaje de memoria Virtual utilizada para el proceso : %.2f%%\n",porcentajeV);
        printf("----------------------------------------------------------------------------\n");
                
        }
    }


    

    
    return 0;
}