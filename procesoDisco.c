#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
	
execlp("/bin/sh", "sh", "-c", "df -h | awk '{print $3, $4, $5}'", NULL);
	return 0;
};