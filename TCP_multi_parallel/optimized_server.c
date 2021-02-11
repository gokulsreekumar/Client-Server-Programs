// //Example code: A simple server side code, which echos back the received message. 
// //Handle multiple socket connections with select and fd_set on Linux  
// #include <stdio.h>  
// #include <string.h>   //strlen  
// #include <stdlib.h>  
// #include <errno.h>  
// #include <unistd.h>   //close  
// #include <arpa/inet.h>    //close  
// #include <sys/types.h>  
// #include <sys/socket.h>  
// #include <netinet/in.h>  
// #include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros  
     
// #define TRUE   1  
// #define FALSE  0  
// #define PORT 8888  
     
// int main(int argc , char *argv[])   
// {   
//     int opt = TRUE;   
//     int master_socket , addrlen , new_socket , client_socket[30] ,  
//           max_clients = 30 , activity, i , valread , sd;   
//     int max_sd;   
//     struct sockaddr_in address;   
         
//     char buffer[1025];  //data buffer of 1K  
         
//     //set of socket descriptors  
//     fd_set readfds;   
         
//     //a message  
//     char *message = "ECHO Daemon v1.0 \r\n";   
     
//     //initialise all client_socket[] to 0 so not checked  
//     for (i = 0; i < max_clients; i++)   
//     {   
//         client_socket[i] = 0;   
//     }   
         
//     //create a master socket  
//     if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)   
//     {   
//         perror("socket failed");   
//         exit(EXIT_FAILURE);   
//     }   
     
//     //set master socket to allow multiple connections ,  
//     //this is just a good habit, it will work without this  
//     if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,  
//           sizeof(opt)) < 0 )   
//     {   
//         perror("setsockopt");   
//         exit(EXIT_FAILURE);   
//     }   
     
//     //type of socket created  
//     address.sin_family = AF_INET;   
//     address.sin_addr.s_addr = INADDR_ANY;   
//     address.sin_port = htons( PORT );   
         
//     //bind the socket to localhost port 8888  
//     if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)   
//     {   
//         perror("bind failed");   
//         exit(EXIT_FAILURE);   
//     }   
//     printf("Listener on port %d \n", PORT);   
         
//     //try to specify maximum of 3 pending connections for the master socket  
//     if (listen(master_socket, 3) < 0)   
//     {   
//         perror("listen");   
//         exit(EXIT_FAILURE);   
//     }   
         
//     //accept the incoming connection  
//     addrlen = sizeof(address);   
//     puts("Waiting for connections ...");   
         
//     while(TRUE)   
//     {   
//         //clear the socket set  
//         FD_ZERO(&readfds);   
     
//         //add master socket to set  
//         FD_SET(master_socket, &readfds);   
//         max_sd = master_socket;   
             
//         //add child sockets to set  
//         for ( i = 0 ; i < max_clients ; i++)   
//         {   
//             //socket descriptor  
//             sd = client_socket[i];   
                 
//             //if valid socket descriptor then add to read list  
//             if(sd > 0)   
//                 FD_SET( sd , &readfds);   
                 
//             //highest file descriptor number, need it for the select function  
//             if(sd > max_sd)   
//                 max_sd = sd;   
//         }   
     
//         //wait for an activity on one of the sockets , timeout is NULL ,  
//         //so wait indefinitely  
//         activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);   
       
//         if ((activity < 0) && (errno!=EINTR))   
//         {   
//             printf("select error");   
//         }   
             
//         //If something happened on the master socket ,  
//         //then its an incoming connection  
//         if (FD_ISSET(master_socket, &readfds))   
//         {   
//             if ((new_socket = accept(master_socket,  
//                     (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)   
//             {   
//                 perror("accept");   
//                 exit(EXIT_FAILURE);   
//             }   
             
//             //inform user of socket number - used in send and receive commands  
//             printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs 
//                   (address.sin_port));   
           
//             //send new connection greeting message  
//             if( send(new_socket, message, strlen(message), 0) != strlen(message) )   
//             {   
//                 perror("send");   
//             }   
                 
//             puts("Welcome message sent successfully");   
                 
//             //add new socket to array of sockets  
//             for (i = 0; i < max_clients; i++)   
//             {   
//                 //if position is empty  
//                 if( client_socket[i] == 0 )   
//                 {   
//                     client_socket[i] = new_socket;   
//                     printf("Adding to list of sockets as %d\n" , i);   
                         
//                     break;   
//                 }   
//             }   
//         }   
             
//         //else its some IO operation on some other socket 
//         for (i = 0; i < max_clients; i++)   
//         {   
//             sd = client_socket[i];   
                 
//             if (FD_ISSET( sd , &readfds))   
//             {   
//                 //Check if it was for closing , and also read the  
//                 //incoming message  
//                 if ((valread = read( sd , buffer, 1024)) == 0)   
//                 {   
//                     //Somebody disconnected , get his details and print  
//                     getpeername(sd , (struct sockaddr*)&address , \ 
//                         (socklen_t*)&addrlen);   
//                     printf("Host disconnected , ip %s , port %d \n" ,  
//                           inet_ntoa(address.sin_addr) , ntohs(address.sin_port));   
                         
//                     //Close the socket and mark as 0 in list for reuse  
//                     close( sd );   
//                     client_socket[i] = 0;   
//                 }   
                     
//                 //Echo back the message that came in  
//                 else 
//                 {   
//                     //set the string terminating NULL byte on the end  
//                     //of the data read  
//                     buffer[valread] = '\0';   
//                     send(sd , buffer , strlen(buffer) , 0 );   
//                 }   
//             }   
//         }   
//     }   
         
//     return 0;   
// }   
/*
** selectserver.c -- a cheezy multiperson chat server
** Source: beej.us/guide/bgnet/examples/selectserver.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT "8888"   // port we're listening on

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number

    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[256];    // buffer for client data
    int nbytes;

	char remoteIP[INET6_ADDRSTRLEN];

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

	struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

	// get us a socket and bind it
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
		fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
		exit(1);
	}
	
	for(p = ai; p != NULL; p = p->ai_next) {
    	listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0) { 
			continue;
		}
		
		// lose the pesky "address already in use" error message
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
			close(listener);
			continue;
		}

		break;
	}

	// if we got here, it means we didn't get bound
	if (p == NULL) {
		fprintf(stderr, "selectserver: failed to bind\n");
		exit(2);
	}

	freeaddrinfo(ai); // all done with this

    // listen
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(3);
    }

    // add the listener to the master set
    FD_SET(listener, &master);

    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one

    // main loop
    for(;;) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == listener) {
                    // handle new connections
                    addrlen = sizeof remoteaddr;
					newfd = accept(listener,
						(struct sockaddr *)&remoteaddr,
						&addrlen);

					if (newfd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) {    // keep track of the max
                            fdmax = newfd;
                        }
                        printf("selectserver: new connection from %s on "
                            "socket %d\n",
							inet_ntop(remoteaddr.ss_family,
								get_in_addr((struct sockaddr*)&remoteaddr),
								remoteIP, INET6_ADDRSTRLEN),
							newfd);
                    }
                } else {
                    // handle data from a client
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);
                        } else {
                            perror("recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                    } else {
                        // we got some data from a client
                        for(j = 0; j <= fdmax; j++) {
                            // send to everyone!
                            if (FD_ISSET(j, &master)) {
                                // except the listener and ourselves
                                if (j != listener && j != i) {
                                    if (send(j, buf, nbytes, 0) == -1) {
                                        perror("send");
                                    }
                                }
                            }
                        }
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!
    
    return 0;
}