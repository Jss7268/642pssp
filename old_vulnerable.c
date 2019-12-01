#include<stdio.h> 
#include<stdlib.h> 
#include<sys/wait.h> 
#include<unistd.h> 

char overflow(char *input) {
	char buffer[5];
//	char retValue = 0;
	unsigned char can0 = buffer[6];
	printf("%d\n", can0);
	strcpy(buffer, input);
	//printf("buffer: %s\n", buffer);
	return 0;
}

int createChild(char* input) {
	pid_t cpid; 
    int status;
    if (fork()== 0) {
	    printf("overflow: %d\n", overflow(input));
	    exit(0);
    } else {
        cpid = wait(&status); 
		if (WIFSIGNALED(status)) {
    		if (WTERMSIG(status) == SIGABRT){
				printf("seg faulted.\n");
				return 1;
			}
		}
		return 0;
	}	
}


int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Improper usage. Requires input argument");
		return 0;
	}

	int childStatus = 1;
	while (childStatus) {
		childStatus = createChild(argv[1]);
	}
  
    return 0; 
} 
