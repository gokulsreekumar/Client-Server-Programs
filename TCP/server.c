#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>     // socket close() defn

// Socket Specific Imports

#include <sys/types.h>  // for type definitions used int socket.h and in.h
#include <sys/socket.h> // all network socket related functions and structures (eg. sockaddr)

#include <netinet/in.h> // for sockaddr_in
#include<arpa/inet.h>   // for inet_ntoa

#define PORT 8080 

int main(int argc, char const *argv[]) 
{ 
	int server_fd, new_socket, n; 
	struct sockaddr_in server_address, client_address; 
	char buffer[1024] = {0}; 
	
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
	if(listen(server_fd, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
	printf("Listening on Port: 8080\n");

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

	// Communication send/recv
	while(1) {
		bzero(buffer, 1024);
		n = recv( new_socket , buffer, 1024, 0); 
		if(n<0) {
			printf("Reading Error!\n");
		}
		printf("Client: %s\n", buffer);
		bzero(buffer, 1024);
		printf("Enter Server's Message:");
		fgets(buffer, 1024, stdin);
		if(strncmp(buffer, "close", 5)==0) {
			close(new_socket);
			break;
		}
		send(new_socket , buffer , strlen(buffer) , 0 ); 
	}

	// Close Server
	close(server_fd);
	printf("Server Closed.\n");
	return 0; 
} 

