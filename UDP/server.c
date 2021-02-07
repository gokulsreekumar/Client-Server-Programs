#include<stdio.h>	//printf
#include<string.h>  //memset
#include<stdlib.h>  //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>

#include <unistd.h> // socket close() defn

#define BUFLEN 512	//Max length of buffer
#define PORT 8888	//The port on which to listen for incoming data

void error(char *s)
{
	perror(s);
	exit(1);
}

/*
    Flow of Server Set Up:
    socket() 
     |
    bind()
     |
    recvfrom() / sendto()

*/


int main(void)
{
	struct sockaddr_in si_server, si_client;
	
	int socket_udp, i, slen = sizeof(si_client) , recv_len;
	char buf[BUFLEN];
	
	//create a UDP socket
	if ((socket_udp=socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		error("socket");
	}
	
	// zero out the structure
	memset((char *) &si_server, 0, sizeof(si_server));
	
	si_server.sin_family = AF_INET;
	si_server.sin_port = htons(PORT);
	si_server.sin_addr.s_addr = htonl(INADDR_ANY);
	
	//bind socket to port
	if( bind(socket_udp , (struct sockaddr*)&si_server, sizeof(si_server) ) == -1)
	{
        close(socket_udp);
		error("bind");
	}
	
	//keep listening for data
	while(1)
	{
		printf("Waiting for data...\n\n");
		fflush(stdout);

        bzero(buf, BUFLEN);
		//try to receive some data, this is a blocking call
        // WE CAN GIVE A FLAG OF MSG_WAITALL here instead of ZERO for Waiting for recieving all the message
		if ((recv_len = recvfrom(socket_udp, buf, BUFLEN, MSG_WAITALL, (struct sockaddr *) &si_client, &slen)) == -1)
		{
			error("recvfrom()");
		}
		
		//print details of the client and the data received
        printf("\n--------- DATA PACKET ------------------\n");
		printf("Received Data Packet from %s:%d\n", inet_ntoa(si_client.sin_addr), ntohs(si_client.sin_port));
		printf("Data: %s\n" , buf);
        printf("----------------------------------------\n\n");
		
		//now reply the client with the same data
		if (sendto(socket_udp, buf, recv_len, 0, (struct sockaddr*) &si_client, slen) == -1)
		{
			error("sendto()");
		}
        bzero(buf, BUFLEN);
	}

	close(socket_udp);
	return 0;
}