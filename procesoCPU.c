#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    FILE *fp;
    char buffer[1024];
    long double a[4], b[4], loadavg;
    const char* sep = " \t\n";

    fp = fopen("/proc/stat", "r");
    if (fp == NULL) {
        printf("No se pudo abrir el archivo /proc/stat\n");
        exit(1);
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (strstr(buffer, "cpu")) {
            char* token = strtok(buffer, sep);
            for(int i = 0; i < 4; i++) {
                token = strtok(NULL, sep);
                a[i] = strtold(token, NULL);
            }
            break;
        }
    }

    fclose(fp);
    sleep(1);

    fp = fopen("/proc/stat", "r");
    if (fp == NULL) {
        printf("No se pudo abrir el archivo /proc/stat\n");
        exit(1);
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (strstr(buffer, "cpu")) {
            char* token = strtok(buffer, sep);
            for(int i = 0; i < 4; i++) {
                token = strtok(NULL, sep);
                b[i] = strtold(token, NULL);
            }
            break;
        }
    }

    fclose(fp);

    loadavg = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));
    loadavg *= 100;

    printf("CPU utilization: %.2Lf%%\n", loadavg);

    return 0;
}