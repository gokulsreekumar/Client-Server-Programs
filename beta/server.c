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

struct message {
    int type;
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

    //memset(server_addr->sin_zero, "", sizeof server_addr->sin_zero);

    // Bind
    if (bind(*sockfd, (struct sockaddr *)server_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("Unable to bind");
        exit(1);
    }
    printf("TCP Server Bound to Port: %d\n", PORT);
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
    }
}

void send_recv(int i, fd_set *master, int sockfd, int fdmax) 
{
    int recv_size, j;
    char recv_buf[BUFSIZE], buf[BUFSIZE];
    if ((recv_size = recv(i, recv_buf, BUFSIZE, 0)) <= 0)
    {
        if (recv_size == 0)
            printf("Socket with FD: %d Hung Up\n", i);
        else
            perror("recv");

        // Close the FD 
        // Clear the Socket from the Master FDs Set
        close(i);
        FD_CLR(i, master);
    }
    else
    {
        for (j = 0; j <= fdmax; j++)
        {
            // Broadcast the Recieved Message to All
            if (FD_ISSET(j, master))
            {
                if (j != sockfd && j != i)
                {
                    if (send(j, recv_buf, recv_size, 0) == -1)
                    {
                        perror("send");
                    }
                }
            }
        }
        printf("%s\n", recv_buf);
        bzero(recv_buf, BUFSIZE);
    }
}