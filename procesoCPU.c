#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdint.h>

#define BUFFER_SIZE 1024
#define NUM_CPU_TIMES 10
#define CPU_LINE_PREFIX "cpu "

typedef struct {
    int pid;
    char name[256];
    unsigned long utime;
    unsigned long stime;
} ProcessInfo;


// Función para leer los tiempos de CPU desde el archivo /proc/stat
int leer_tiempos_cpu(unsigned long long int tiempos[]) {
  int fd, n;
  char buffer[BUFFER_SIZE];

  struct stat st;
  if (stat("/proc/stat", &st) == -1) {
    perror("Error al obtener información sobre el archivo /proc/stat");
    return -1;
  }
  if (!S_ISREG(st.st_mode)) {
    fprintf(stderr, "El archivo /proc/stat no es un archivo regular\n");
    return -1;
  }
  if (access("/proc/stat", R_OK) == -1) {
    perror("Error al verificar los permisos de lectura del archivo /proc/stat");
    return -1;
  }

  
  fd = open("/proc/stat", O_RDONLY);
  if (fd == -1) {
    perror("Error al abrir el archivo /proc/stat");
    return -1;
  }

  n = read(fd, buffer, sizeof(buffer));
  if (n == -1) {
    perror("Error al leer el archivo /proc/stat");
    close(fd);
    return -1;
  }

  close(fd);

  // Analizar la línea de CPU y extraer los tiempos
  if (sscanf(buffer + strlen(CPU_LINE_PREFIX), "%llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
      &tiempos[0], &tiempos[1], &tiempos[2], &tiempos[3],
      &tiempos[4], &tiempos[5], &tiempos[6], &tiempos[7],
      &tiempos[8], &tiempos[9]) != NUM_CPU_TIMES) {
    fprintf(stderr, "Error al analizar la línea de CPU en el archivo /proc/stat\n");
    return -1;
  }

  return 0;
}

// Función para calcular el porcentaje total de utilización de la CPU
float calcular_porcentaje_cpu() {
  unsigned long long int tiempos1[NUM_CPU_TIMES] = {0}, tiempos2[NUM_CPU_TIMES] = {0};
  unsigned long long int tiempos3[NUM_CPU_TIMES] = {0}, tiempos4[NUM_CPU_TIMES] = {0};
  unsigned long long int tiempos5[NUM_CPU_TIMES] = {0};
  unsigned long long int diferencia_total = 0, diferencia_idle = 0;
  float porcentaje_utilizacion = 0;


  if (leer_tiempos_cpu(tiempos1) == -1) {
    return -1;
  }

  sleep(1);

  if (leer_tiempos_cpu(tiempos2) == -1) {
    return -1;
  }

  sleep(1);

  if (leer_tiempos_cpu(tiempos3) == -1) {
    return -1;
  }

  sleep(1);

  if (leer_tiempos_cpu(tiempos4) == -1) {
    return -1;
  }

  sleep(1);

  if (leer_tiempos_cpu(tiempos5) == -1) {
    return -1;
  }

  for (int i = 0; i < NUM_CPU_TIMES; ++i) {
    diferencia_total += tiempos5[i] - tiempos1[i];
  }

  diferencia_idle = tiempos5[3] - tiempos1[3];

  porcentaje_utilizacion = 100.0 * (1.0 - ((float)diferencia_idle / diferencia_total));

  return porcentaje_utilizacion;
}

int get_process_info(int pid, ProcessInfo *info) {
    char filename[256];
    FILE *file;
    char buffer[BUFFER_SIZE];

    sprintf(filename, "/proc/%d/stat", pid);
    file = fopen(filename, "r");
    if (!file) {
        perror("fopen");
        return -1;
    }

    if (fgets(buffer, BUFFER_SIZE, file) == NULL) {
        fclose(file);
        return -1;
    }

    fclose(file);

    // Parse the content of /proc/[pid]/stat
    sscanf(buffer, "%d %s %*c %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %lu %lu",
        &info->pid, info->name, &info->utime, &info->stime);

    return 0;
}

float calculate_cpu_usage(unsigned long utime, unsigned long stime, unsigned long prev_utime, unsigned long prev_stime, unsigned long delta_time, long num_cpus) {
    unsigned long total_time = utime + stime;
    unsigned long prev_total_time = prev_utime + prev_stime;

    unsigned long total_time_diff = total_time - prev_total_time;

    unsigned long delta_time_seconds = delta_time / 1000000000; 

    float cpu_rate = (float)total_time_diff / (delta_time_seconds * num_cpus);

    float usage = cpu_rate * 100.0;

    return usage;
}


int main(int argc, char *argv[]) {
    if (argc == 1) {
        float cpu_usage = calcular_porcentaje_cpu();
        if (cpu_usage == -1) {
            fprintf(stderr, "No se pudo calcular el porcentaje de utilización de la CPU\n");
            return 1;
        }

        printf("Porcentaje de utilización de la CPU: %.2f%%\n", cpu_usage);
    } else if (argc == 2) {
        int pid;
        ProcessInfo info;
        unsigned long prev_utime = 0, prev_stime = 0;
        unsigned long delta_time = 0;
        float usage;
        int i;
        struct sysinfo sys_info;
        sysinfo(&sys_info);
        long num_cpus = sys_info.procs;
        

        pid = atoi(argv[1]);

        for (i = 0; i < 5; ++i) {
            if (get_process_info(pid, &info) == -1) {
                fprintf(stderr, "No se pudo obtener información del proceso con PID %d\n", pid);
                return 1;
            }

            struct timespec start, current;
            clock_gettime(CLOCK_MONOTONIC, &start);

            sleep(60); 

            clock_gettime(CLOCK_MONOTONIC, &current);
            delta_time = (current.tv_sec - start.tv_sec) * 1000000000 + (current.tv_nsec - start.tv_nsec);

            usage = calculate_cpu_usage(info.utime, info.stime, prev_utime, prev_stime, delta_time, num_cpus);

            prev_utime = info.utime;
            prev_stime = info.stime;

            printf("Minuto %d - Porcentaje de utilización del proceso %d %s: %.2f%%\n", i+1, info.pid, info.name, usage);
        }
    }

    return 0;
}
