#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>     // socket close() defn

// Socket Specific Imports

#include <sys/types.h>  // for type definitions used int socket.h and in.h
#include <sys/socket.h> // all network socket related functions and structures (eg. sockaddr)

#include <netinet/in.h> // for sockaddr_in
#include <arpa/inet.h>   // for inet_ntoa

#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define SERVE_BACKLOG 3

#define MAX_THREADS 100


int sockets[MAX_THREADS]={0};

void* communication(void* new_socket);


int main(int argc, char const *argv[]) 
{ 
	int server_fd, new_socket, n; 
	struct sockaddr_in server_address, client_address; 
	char buffer[BUFFER_SIZE] = {0}; 
	
	// Create TCP Socket
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	printf("TCP Socket Created!\n");

	// Make server_addr 
	// Address internet: (IP,port)
	server_address.sin_family = AF_INET; 
	server_address.sin_addr.s_addr = INADDR_ANY; 
	server_address.sin_port = htons( PORT ); 
	
	// Binding socket to the server_addr
	if (bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	}
	printf("Socket bound to 0.0.0.0: 8080\n");

	// Listening at Port
	if(listen(server_fd, SERVE_BACKLOG) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
	printf("Listening on Port: 8080\n");
	int i = 0;
	while(1) {
		// Awaiting Incoming Connections
		// Accepting Connections, yielding new_socket
		int addrlen = sizeof(client_address); 
		if ((new_socket = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t*)&addrlen))<0) 
		{ 
			perror("accept"); 
			exit(EXIT_FAILURE); 
		} 
		// inet_ntoa() : Convert Internet number in IN to ASCII representation. 
		// The return value is a pointer to an internal array containing the string.
		// ntohs() => internet number to 16 bit (PORT)
		// ntohl() => internet number to 32 bit (IP Address)
		printf("Connected to %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

		pthread_t t;
		int* pclient = malloc(sizeof(int));
		*pclient = new_socket;
		sockets[i++] = new_socket;
		// // Communication send/recv
		// communication(new_socket);

		pthread_create(&t, NULL, communication, pclient);

	}
	// Close Server
	close(server_fd);
	printf("Server Closed.\n");
	return 0; 
} 

void* broadcast(char* buffer, int socketfd) {
	for(int i = 0; i<50; i++) {
		if(sockets[i]!=0) {
			char name[5] = "Name:";
			strcat(name, buffer);
			send(sockets[i], name, strlen(name) , 0);
		} else {
			break;
		}
	}
}

void* communication(void* pclient) {
	int new_socket = *((int*)pclient);
	free(pclient);
	int n;
	char buffer[BUFFER_SIZE] = {0}; 
	while(1) {
		bzero(buffer, BUFFER_SIZE);
		n = recv( new_socket , buffer, BUFFER_SIZE, 0); 
		if(n<0) {
			printf("Reading Error!\n");
		}
		printf("Client: %s\n", buffer);
		if(strncmp(buffer, "close", 5)==0) {
			printf("Client with Socketfd: %d disconnected!\n", new_socket);
			bzero(buffer, BUFFER_SIZE);
			return NULL;
		}
		// bzero(buffer, BUFFER_SIZE);
		// printf("Enter Server's Message:");
		// fgets(buffer, BUFFER_SIZE, stdin);
		// if(strncmp(buffer, "close", 5)==0) {
		// 	close(new_socket);
		// 	break;
		// }
		send(new_socket , buffer , strlen(buffer) , 0); 
	}
	return NULL;
}

