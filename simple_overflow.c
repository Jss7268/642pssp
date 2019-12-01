#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
int main(int argc, char **argv) {
	char buf[5];
	printf("%d\n", buf[6]);
	strcpy(buf, argv[1]);
}  
