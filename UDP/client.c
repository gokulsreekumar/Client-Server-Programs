#include<stdio.h>	//printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include <unistd.h> // socket close() defn

// Socket Specific Imports
#include<arpa/inet.h>
#include<sys/socket.h>

#define SERVER "127.0.0.1"
#define BUFLEN 512	//Max length of buffer
#define PORT 8888	//The port on which to send data

void error(char *s)
{
	perror(s);
	exit(1);
}

/*
    Flow of Server Set Up:
    socket() 
     |
	sendto()
	 |
    recvfrom()

*/

int main(void)
{
	struct sockaddr_in si_client;
	int socket_udp, i, slen=sizeof(si_client);
	char buf[BUFLEN];
	char message[BUFLEN];

	if ((socket_udp=socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		error("socket");
	}

	memset((char *) &si_client, 0, sizeof(si_client));

	si_client.sin_family = AF_INET;
	si_client.sin_port = htons(PORT);
	if (inet_aton(SERVER , &si_client.sin_addr) == 0) 
	{
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	while(1)
	{
		printf("Enter message : ");
		fgets(message, 512, stdin);
		
        if(strncmp(message, "close", 5)==0) {
            close(socket_udp);
            exit(0);
        }
		//SendTo() for sending UDP
		if (sendto(socket_udp, message, strlen(message) , 0 , (struct sockaddr *) &si_client, slen)==-1)
		{
			error("sendto()");
		}
		
		//clear the buffer
		memset(buf,'\0', BUFLEN);

		// BLOCKING CALL FOR RECIEVING
		// try to receive some data, this is a blocking call
		if (recvfrom(socket_udp, buf, BUFLEN, 0, (struct sockaddr *) &si_client, &slen) == -1)
		{
			error("recvfrom()");
		}
		
		puts(buf);
	}

	close(socket_udp);
	return 0;
}