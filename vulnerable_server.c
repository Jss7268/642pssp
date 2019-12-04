#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include<sys/wait.h> 
#include <sys/signal.h>

#define OK_STR ("OK\n")
#define STACK_SMASH_STR ("SMASH\n")
#define SEG_FAULT_STR ("SIGSEG\n")

enum status{OK, SMASHED, SEG_FAULTED}; 

// vulnerable method where stack buffer overflow can occur
// if the socket receives too much data
int overflow(int socket) {
	char buffer[5];
	unsigned int can0 = buffer[6];

	// this print statement show how the canary value is static for SSP and dynamic for P-SSP
	printf("%d\n", can0);
	int val = recv(socket, buffer, 1024, 0);
	return val;
}

// this method spawns a child to "work" on the new connection.
// if the child fails, then return a specific non-zero code.
enum status createChild(int socket) {
	pid_t cpid; 
	int status;
	if (fork()== 0) {
		overflow(socket);
		exit(0);
	} else {
		cpid = wait(&status); 
		if (WIFSIGNALED(status)) {
			if (WTERMSIG(status) == SIGSEGV){
				// on seg fault
				return SEG_FAULTED;
			} else if (WTERMSIG(status) == SIGABRT) {
				// on stack smashing detected
				return SMASHED;
			}
		}
		return OK;
	}	
}


int main(int argc, char const *argv[]) { 
	int serverfd, newSocket; 
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 
	
	if (argc < 2) {
		printf("Please specify port. 8080 for SSP and 1234 for P-SSP");
		return 1;
	}
	char *endptr;
	long PORT = strtoll(argv[1], &endptr, 10);
 
	// Creating socket file descriptor 
	if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	   
	// Forcefully attaching socket to the port 8080 
	if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
	   
	// Forcefully attaching socket to the port 8080 
	if (bind(serverfd, (struct sockaddr *)&address, sizeof(address)) < 0) { 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	if (listen(serverfd, 3) < 0) { 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 

	// loop and wait for new connections
	while (1) {
		if ((newSocket = accept(serverfd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) { 
			perror("accept"); 
			exit(EXIT_FAILURE); 
		}

		enum status childStatus = createChild(newSocket);
		
		// send message depending on the status
		char *message;
		switch (childStatus) {
			case OK:
				send(newSocket, OK_STR, strlen(OK_STR), 0);
				break;
			case SEG_FAULTED:
				send(newSocket, SEG_FAULT_STR, strlen(SEG_FAULT_STR), 0);
				break;
			case SMASHED:
				// do nothing
				break;
		}
		
		close(newSocket);
	} 
	return 0; 
} 
