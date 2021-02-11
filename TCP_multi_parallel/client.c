// #include <stdio.h> 
// #include <string.h> 
// #include <unistd.h> 	// socket close() defn

// #include <sys/socket.h> 
// #include <arpa/inet.h>  // for inet_ntoa

// #include <pthread.h>

// #define PORT 8888   	// PORT to connect to
// #define LO "127.0.0.1" 	// LOOPBACK ADDRESS FOR CONNECTING TO LOCAL HOST 

// int main(int argc, char const *argv[]) 
// { 
// 	int socket_client = 0, n; 
// 	struct sockaddr_in serv_addr; 
// 	char buffer[1024] = {0}; 

// 	// Create Socket
// 	if ((socket_client = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
// 	{ 
// 		printf("\n Socket creation error \n"); 
// 		return -1; 
// 	}

// 	// Make Address
// 	serv_addr.sin_family = AF_INET; 
// 	serv_addr.sin_port = htons(PORT); 
	
// 	// Convert IPv4 and IPv6 addresses from text to binary form 
// 	if(inet_pton(AF_INET, LO , &serv_addr.sin_addr)<=0) 
// 	{ 
// 		printf("\nInvalid address/ Address not supported \n"); 
// 		return -1; 
// 	} 

// 	// Connect()
// 	if (connect(socket_client, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
// 	{ 
// 		printf("\nConnection Failed \n"); 
// 		return -1; 
// 	}
	
// 	// Communication
// 	while(1) {
// 		bzero(buffer, 1024);
// 		printf("Enter Client's Message: ");
// 		fgets(buffer, 1024, stdin);
// 		char bye[5] = "close";
// 		if(strncmp(buffer, "close", 5)==0) {
// 			n = send(socket_client , bye, 1024, 0); 
// 			// close()
// 			close(socket_client);
// 			break;
// 		}
// 		n = send(socket_client , buffer, 1024, 0); 
// 		if(n<0) {
// 			printf("Reading Error!\n");
// 		}
// 		bzero(buffer, 1024);
// 		n = recv(socket_client , buffer, 1024, 0); 

// 		printf("Server: %s\n", buffer);
// 		bzero(buffer, 1024);

// 	}
// 	printf("Client Closed Connection.\n"); 
// 	return 0; 
// } 

/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "8888" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	if (argc != 2) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	}

	buf[numbytes] = '\0';

	printf("client: received '%s'\n",buf);

	close(sockfd);

	return 0;
}
