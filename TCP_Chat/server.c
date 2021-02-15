#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 8080
#define BUFSIZE 1024

#define MAX_NUMBER_CLIENTS 1024
char name_directory[MAX_NUMBER_CLIENTS][32];

// Message Structure
// 0 - Message
// 1 - name
// 2 - end
// 3 - namelist
struct message
{
    int type;
    char name[32];
    char data[BUFSIZE];
};

// Server Set Up: Socket Creation, Binding, Listening
void server_set_up(int *sockfd, struct sockaddr_in *server_addr);
// Accept a Client Connection
void connection_accept(fd_set *master, int *fdmax, int sockfd, struct sockaddr_in *client_addr);
// Handle Sending and recieving of Data
void send_recv(int i, fd_set *master, int sockfd, int fdmax);

int main()
{
    fd_set master, read_fds;

    int fdmax, i;
    int sockfd = 0;

    struct sockaddr_in server_addr, client_addr;

    // Initialization
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    server_set_up(&sockfd, &server_addr);

    // Add Server Socket to Master Set of Sockets
    FD_SET(sockfd, &master);
    // Maximum Socket Number as of now.
    fdmax = sockfd;

    // Initialize the Name directory as Empty
    memset(name_directory, '\0', sizeof(name_directory));

    while (1)
    {
        // Taking a Copy of FD set.
        read_fds = master;

        // Checking for any Activity (Rewrites read_fds)
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1)
        {
            perror("select");
            exit(4);
        }

        // Finding the Socket which requires the Attention
        for (i = 0; i <= fdmax; i++)
        {
            if (FD_ISSET(i, &read_fds))
            {
                if (i == sockfd)
                    // If the server socket is having any new connections.
                    // Add new Connection SocketFD to Master Set of FDs
                    connection_accept(&master, &fdmax, sockfd, &client_addr);
                else
                    // Activity in some connected client socket.
                    send_recv(i, &master, sockfd, fdmax);
            }
        }
    }
    return 0;
}

void server_set_up(int *sockfd, struct sockaddr_in *server_addr)
{
    if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket");
        exit(1);
    }

    // Make Address
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(PORT);
    server_addr->sin_addr.s_addr = INADDR_ANY;
    // Bind
    if (bind(*sockfd, (struct sockaddr *)server_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("Unable to bind");
        exit(1);
    }
    printf("TCP Server Bound to Port: %d\n", PORT);

    // Options
    int flag = 1;
	if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) == -1) {
		perror("error in setsockopt");
		exit(1);
	}

    // Listen
    if (listen(*sockfd, 10) == -1)
    {
        perror("listen");
        exit(1);
    }
    printf("TCP Server Waiting for Client Requests...\n");
    fflush(stdout);
}

void connection_accept(fd_set *master, int *fdmax, int sockfd, struct sockaddr_in *client_addr)
{
    socklen_t addrlen = sizeof(struct sockaddr_in);
    int newsockfd;

    char list_message[32] = "The List of People Online are:";
    char name_message[32];

    struct message send_message;

    if ((newsockfd = accept(sockfd, (struct sockaddr *)client_addr, &addrlen)) == -1)
    {
        perror("accept");
        exit(1);
    }
    else
    {
        // Add the new Socket to the Master Set.
        FD_SET(newsockfd, master);
        // Update the maximum fd if need be.
        if (newsockfd > *fdmax)
            *fdmax = newsockfd;
        printf("New connection at %s : %d \n", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));

        // Send the List of Online Members
        strcpy(send_message.data, "\0");
        for (int i = 0; i < *(fdmax); i++)
        {
            if (FD_ISSET(i, master) && i != newsockfd)
            {
                if (name_directory[i][0] != '\0')
                {
                    send_message.type = 3;
                    strcpy(send_message.name, name_directory[i]);
                    send(newsockfd, &send_message, sizeof(struct message), 0);
                }
            }
        }
    }
}

void send_recv(int i, fd_set *master, int sockfd, int fdmax)
{
    int recv_size, j;
    struct message recv_message, send_message;
    char recv_buf[BUFSIZE];

    if ((recv_size = recv(i, &recv_message, sizeof(struct message), 0)) <= 0)
    {
        if (recv_size == 0)
            printf("Socket with FD: %d Hung Up\n", i);
        else
            perror("recv");

        // Remove Name Directory Entry
        strcpy(name_directory[i], "\0");

        // Close the FD
        // Clear the Socket from the Master FDs Set
        close(i);
        FD_CLR(i, master);
    }
    else
    {
        // Prepare message to send
        send_message.type = recv_message.type;
        strcpy(send_message.name, recv_message.name);
        strcpy(send_message.data, recv_message.data);

        if (recv_message.type == 1)
        {
            // Add the name to Directory
            strcpy(name_directory[i], recv_message.name);
        }

        for (j = 0; j <= fdmax; j++)
        {
            // Broadcast the Recieved Message to All
            if (FD_ISSET(j, master))
            {
                if (j != sockfd && j != i)
                {
                    if (send(j, &send_message, sizeof(struct message), 0) == -1)
                    {
                        perror("send");
                    }
                }
            }
        }
        if (recv_message.type == 0)
        {
            printf("%s > %s\n", recv_message.name, recv_message.data);
        }
        else if (recv_message.type == 2)
        {
            printf("%s left the chat\n", recv_message.name);
        }
        bzero(recv_buf, BUFSIZE);
    }
}