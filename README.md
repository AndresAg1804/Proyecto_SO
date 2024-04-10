# Inspector de Recursos
## Integrantes 
- Anner Angulo Gutiérrez
- Gabriel Robleto Jerez

## Descripción
Este proyecto consiste en desarrollar una suite de aplicaciones para obtener estadísticas
sobre los recursos del sistema, incluyendo la utilización de CPU, memoria y disco. 
El programa principal deberá recibir como argumento en la línea de comandos 
el tipo de recurso del cual se desea obtener información.
El programa principal creará un subproceso para obtener la estadística deseada. 
Se deberá crear un programa específico para cada estadística. 
Este subproceso recopilará los detalles de utilización de cada recurso y, utilizando pipes, 
proporcionará la información de regreso al proceso padre para imprimir en pantalla las estadísticas.

## Información a recopilar para cada recurso:
### CPU
- Porcentaje de utilización total (de todos los procesos).
- Porcentaje de utilización para un proceso específico en los últimos 5 minutos (muestra el PID, nombre y porcentaje de utilización).

### Memoria
- Porcentaje de utilización de memoria virtual para todos los procesos (mostrando el PID, nombre y porcentaje de utilización).
- Porcentaje de utilización de memoria real para todos los procesos (mostrando el PID, nombre y porcentaje de utilización).

### Disco
- Porcentaje total de utilización en MiB o GiB.
- Porcentaje total de espacio libre en MiB o GiB.
