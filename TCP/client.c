#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>  // for inet_ntoa
#include <unistd.h> 	// socket close() defn
#include <string.h> 
#define PORT 8080   	// PORT to connect to
#define LO "127.0.0.1" 	// LOOPBACK ADDRESS FOR CONNECTING TO LOCAL HOST 

int main(int argc, char const *argv[]) 
{ 
	int socket_client = 0, n; 
	struct sockaddr_in serv_addr; 
	char buffer[1024] = {0}; 

	// Create Socket
	if ((socket_client = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	}

	// Make Address
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	
	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, LO , &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	// Connect()
	if (connect(socket_client, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	}
	
	// Communication
	while(1) {
		bzero(buffer, 1024);
		printf("Enter Client's Message: ");
		fgets(buffer, 1024, stdin);
		if(strncmp(buffer, "close", 5)==0) {
			// close()
			close(socket_client);
			break;
		}
		n = send(socket_client , buffer, 1024, 0); 
		if(n<0) {
			printf("Reading Error!\n");
		}
		bzero(buffer, 1024);
		n = recv(socket_client , buffer, 1024, 0); 

		printf("Server: %s\n", buffer);
		bzero(buffer, 1024);

	}
	printf("Client Closed Connection.\n"); 
	return 0; 
} 

