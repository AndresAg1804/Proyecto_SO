#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#define NUM_CPU_TIMES 10
#define CPU_LINE_PREFIX "cpu "

// Función para leer los tiempos de CPU desde el archivo /proc/stat
int leer_tiempos_cpu(unsigned long long int tiempos[]) {
  int fd, n;
  char buffer[1024];

  // Verificar si el archivo existe y si tiene los permisos adecuados
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

  // Abrir el archivo y leer los datos
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

  // Leer los tiempos de CPU para la primera lectura
  if (leer_tiempos_cpu(tiempos1) == -1) {
    return -1;
  }

  // Esperar un breve período de tiempo
  sleep(1);

  // Leer los tiempos de CPU para la segunda lectura
  if (leer_tiempos_cpu(tiempos2) == -1) {
    return -1;
  }

  // Leer los tiempos de CPU para la tercera lectura
  if (leer_tiempos_cpu(tiempos3) == -1) {
    return -1;
  }

  // Leer los tiempos de CPU para la cuarta lectura
  if (leer_tiempos_cpu(tiempos4) == -1) {
    return -1;
  }

  // Leer los tiempos de CPU para la quinta lectura
  if (leer_tiempos_cpu(tiempos5) == -1) {
    return -1;
  }

  // Calcular la diferencia total de tiempos de CPU
  for (int i = 0; i < NUM_CPU_TIMES; ++i) {
    diferencia_total += tiempos5[i] - tiempos1[i];
  }

  // Calcular la diferencia de tiempo en idle
  diferencia_idle = tiempos5[3] - tiempos1[3];

  // Calcular el porcentaje de utilización de la CPU
  porcentaje_utilizacion = 100.0 * (1.0 - ((float)diferencia_idle / diferencia_total));

  return porcentaje_utilizacion;
}

int main() {
  // Calcular el porcentaje total de utilización de la CPU
  float porcentaje_cpu = calcular_porcentaje_cpu();

  // Imprimir el resultado
  if (porcentaje_cpu != -1) {
    printf("Porcentaje total de utilización de la CPU: %.2f%%\n", porcentaje_cpu);
  } else {
    printf("Error al calcular el porcentaje de utilización de la CPU\n");
  }

  return 0;
}