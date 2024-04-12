#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]){

	if(argc==2){
		char *opcion = argv[1];
			if (strcmp(opcion, "used") == 0) {
			execlp("/bin/sh", "sh", "-c", "df -h | awk '{print $1,$3}'", NULL);
		} else if (strcmp(opcion, "avail") == 0) {
			execlp("/bin/sh", "sh", "-c", "df -h | awk '{print $1,$4}'", NULL);
		} else if (strcmp(opcion, "useP") == 0) {
			execlp("/bin/sh", "sh", "-c", "df -h | awk '{print $1,$5}'", NULL);
		}
		else if (strcmp(opcion, "mib")==0){
			execlp("/bin/sh", "sh", "-c", "df -BM | awk '{print $1,$3, $4, $5}'", NULL);
		}
	
	}
		
	execlp("/bin/sh", "sh", "-c", "df -h | awk '{print $1,$3, $4, $5}'", NULL);


	
	
	
	return 0;
};