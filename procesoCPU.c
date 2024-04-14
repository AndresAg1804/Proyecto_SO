#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/sysinfo.h>

#define BUFFER_SIZE 1024

typedef struct {
    int pid;
    char name[256];
    unsigned long utime;
    unsigned long stime;
} ProcessInfo;

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

    // Calcular la diferencia en tiempo de CPU desde la última medición
    unsigned long total_time_diff = total_time - prev_total_time;

    // Calcular la diferencia en tiempo transcurrido desde la última medición
    unsigned long delta_time_seconds = delta_time / 1000000000; // Convertir delta_time a segundos

    // Calcular la tasa de cambio del tiempo de CPU en unidades por segundo
    float cpu_rate = (float)total_time_diff / (delta_time_seconds * num_cpus);

    // Convertir la tasa de cambio a porcentaje de utilización de la CPU
    float usage = cpu_rate * 100.0;

    return usage;
}


int main(int argc, char *argv[]) {
    int pid;
    ProcessInfo info;
    unsigned long prev_utime = 0, prev_stime = 0;
    unsigned long delta_time = 0;
    float usage;
    int i;
    struct sysinfo sys_info;
    sysinfo(&sys_info);
    long num_cpus = sys_info.procs;

    if (argc != 2) {
        fprintf(stderr, "Uso: %s <PID>\n", argv[0]);
        return 1;
    }

    pid = atoi(argv[1]);

    for (i = 0; i < 5; ++i) {
        if (get_process_info(pid, &info) == -1) {
            fprintf(stderr, "No se pudo obtener información del proceso con PID %d\n", pid);
            return 1;
        }

        struct timespec start, current;
        clock_gettime(CLOCK_MONOTONIC, &start);

        sleep(60); // Esperar un minuto

        clock_gettime(CLOCK_MONOTONIC, &current);
        delta_time = (current.tv_sec - start.tv_sec) * 1000000000 + (current.tv_nsec - start.tv_nsec);

        usage = calculate_cpu_usage(info.utime, info.stime, prev_utime, prev_stime, delta_time, num_cpus);

        prev_utime = info.utime;
        prev_stime = info.stime;

        printf("Minuto %d - Porcentaje de utilización del proceso %d %s: %.2f%%\n", i+1, info.pid, info.name, usage);
    }

    return 0;
}
