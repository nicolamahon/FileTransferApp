#include <stdio.h>      	// for IO
#include <string.h>     	// for strlen
#include <sys/socket.h> 	// for socket
#include <arpa/inet.h>  	// for inet_addr
#include <unistd.h>     	// for getuid
#include <stdlib.h> 		// for exit
#include <pthread.h>		// for threads

#include "server.h"


// MAIN
int main(int argc , char *argv[])
{
	int server_sock; 
	int client_sock;
	int connSize; // Size of struct 

	struct sockaddr_in server , client;
	pthread_t thread;
	int client_thread;

	//Create socket
	server_sock = socket(AF_INET , SOCK_STREAM , 0);
	if (server_sock == -1)
	{
		printf("Could not create socket\n");
	}
	else 
	{
		printf("Socket Successfully Created!!\n");
	} 

	// set sockaddr_in variables
	server.sin_port = htons( 8082 ); // Set the prot for communication
	server.sin_family = AF_INET; // Use IPV4 protocol
	server.sin_addr.s_addr = INADDR_ANY; 
	// When INADDR_ANY is specified in the bind call
	// the  socket will be bound to all local interfaces. 
    
     
	//Bind
	if(bind(server_sock,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("Bind issue!!\n");
		return 1;
	} 
	else 
	{
    		printf("Bind Complete!!\n");
    	}

	//Listen for a conection
	listen(server_sock,3); 

	//Accept and incoming connection
	printf("\n-------------------------------------------\n");
	printf("WAITING FOR INCOMING CONNECTION FROM CLIENT\n");
	printf("-------------------------------------------\n\n");

	// set size of socket connection
	connSize = sizeof(struct sockaddr_in);
     
	//accept connection from an incoming client
	while(client_sock = accept(server_sock, (struct sockaddr *)&client, (socklen_t*)&connSize))
	{
		if (client_sock < 0)
		{
			perror("Can't establish connection\n");
			return 1;
		} 
		else 
		{
			printf("Connection from client accepted!!\n");
			// CREATE THREAD for new client connection
			if((client_thread=pthread_create(&thread,NULL,acceptClient,(void*)&client_sock)))
			{
				printf("Error creating client thread...\n");
				exit(EXIT_FAILURE);
			}
		
		}
	}

	return 0;
}



