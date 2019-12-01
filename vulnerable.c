#include<stdio.h> 
#include<stdlib.h> 
#include<sys/wait.h> 
#include<unistd.h> 
#include <string.h>

char overflow(char *input) {
	char buffer[5];
//	char retValue = 0;
	unsigned char can0 = buffer[6];
	printf("%d\n", can0);
	strcpy(buffer, input);
	//printf("buffer: %s\n", buffer);
	return 0;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Improper usage. Requires input argument");
		return 0;
	}
	overflow(argv[1]);
	//printf("overflow: %d\n", overflow(argv[1]));
}
