#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>

//No funciona Bien. Este es el del Porcentaje de utilizacion de un proceso en 5 minutos
double get_cpu_usage(pid_t pid) {
    struct rusage usage;
    struct timeval start, end;
    double total_time, cpu_time;

    gettimeofday(&start, NULL);
    if (getrusage(RUSAGE_CHILDREN, &usage) == -1) {
        perror("getrusage");
        exit(EXIT_FAILURE);
    }
    total_time = usage.ru_utime.tv_sec + usage.ru_stime.tv_sec +
                 (usage.ru_utime.tv_usec + usage.ru_stime.tv_usec) / 1000000.0;

    sleep(5 * 60); // wait for 5 minutes

    gettimeofday(&end, NULL);
    if (getrusage(RUSAGE_CHILDREN, &usage) == -1) {
        perror("getrusage");
        exit(EXIT_FAILURE);
    }
    total_time = end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) / 1000000.0;
    cpu_time = usage.ru_utime.tv_sec + usage.ru_stime.tv_sec +
               (usage.ru_utime.tv_usec + usage.ru_stime.tv_usec) / 1000000.0;

    return (cpu_time / total_time) * 100.0;
}

int main(int argc, char *argv[]) {
    pid_t pid;
    char cmd[256];
    struct stat st;
    double usage;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <PID>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid = atoi(argv[1]);

    if (stat(argv[1], &st) == -1 || !S_ISREG(st.st_mode)) {
        fprintf(stderr, "%s is not a valid PID\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    usage = get_cpu_usage(pid);

    if (usage < 0.0) {
        fprintf(stderr, "Error: could not get CPU usage for PID %d\n", pid);
        exit(EXIT_FAILURE);
    }

    snprintf(cmd, sizeof(cmd), "/proc/%d/status", pid);
    FILE *fp = fopen(cmd, "r");
    if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char name[16];
    fgets(name, sizeof(name), fp);
    fclose(fp);

    name[strcspn(name, "\n")] = '\0';

    printf("Process %s (PID %d) CPU usage: %.2f%%\n", name, pid, usage);

    return 0;
}