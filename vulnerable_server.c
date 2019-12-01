// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include<sys/wait.h> 
#include <sys/signal.h>

#define PORT 8080 
#define OK_STR ("OK\n")
#define STACK_SMASH_STR ("SMASH\n")
#define SEG_FAULT_STR ("SIGSEG\n")

int overflow(int socket) {
	char buffer[5];
	int val = recv(socket, buffer, 1024, 0);

	return val;
}

int createChild(int socket) {
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
				return 1;
			} else if (WTERMSIG(status) == SIGABRT) {
				// on stack smashing detected
				return 2;
			}
		}
		return 0;
	}	
}

int main(int argc, char const *argv[]) { 
	int serverfd, newSocket; 
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 
	   
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

	while (1) {
		if ((newSocket = accept(serverfd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) { 
			perror("accept"); 
			exit(EXIT_FAILURE); 
		}

		int childStatus = createChild(newSocket);
		
		printf("Child status: %d\n", childStatus);
		char *message;
		switch (childStatus) {
			case 0:
				send(newSocket, OK_STR, strlen(OK_STR), 0);
				break;
			case 1:
				send(newSocket, SEG_FAULT_STR, strlen(SEG_FAULT_STR), 0);
				message = SEG_FAULT_STR;
				break;
			case 2:
				// do nothing
				break;
		}
		
		close(newSocket);
	} 
	return 0; 
} 
