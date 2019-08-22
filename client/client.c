#include <stdio.h>
#include <string.h>    	// for strlen..
#include <sys/socket.h>
#include <arpa/inet.h> 	// for inet_addr
#include <unistd.h>    	// for write
#include <sys/stat.h>	// for stat

 
int main(int argc , char *argv[])
{
	int client_sock;
	struct sockaddr_in server;
	struct stat file_buff;
	char client_buff[500];
	char server_buff[500];
	char filename[50];
	char filepath[500];
	char command[100];

	uid_t uid = getuid();
	uid_t gid = getgid();
	uid_t ueid = geteuid();
	uid_t geid = getegid();
	uid_t myUID = 0;

	printf("User ID: %d\nGroup ID: %d\nUser Effective ID: %d\nGroup Effective ID: %d\n", uid, gid, ueid, geid);

	strcpy(filename, argv[1]);
	strcpy(filepath, argv[2]);

	//Create socket
	client_sock = socket(AF_INET , SOCK_STREAM , 0);
	if (client_sock == -1)
	{
		printf("Error creating socket\n");
	} 
	else
	{
		printf("Socket created\n");
	} 

	// set sockaddr_in variables
	server.sin_port = htons( 8082 ); // Port to connect on
	server.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP
	server.sin_family = AF_INET; // IPV4 protocol


	//Connect to server
	if (connect(client_sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		printf("Connect failed. Error\n");
		return 1;
	}

	// Confirm all is going well
	printf("Connected to server ok!!\n");

	// SEND DATA TO SERVER
	printf("\n-------------------------------------------\n");
	printf("\tSENDING DATA TO SERVER\n");
	printf("-------------------------------------------\n\n");

	send(client_sock, filename, strlen(filename), 0);
	sleep(1);
	send(client_sock, filepath, strlen(filepath), 0);
	sleep(1);
	send(client_sock, &uid, sizeof(uid), 0);
	sleep(1);
	send(client_sock, &gid, sizeof(gid), 0);
	sleep(1);

	// prepare to send file
	FILE *file_open = fopen(filename, "r");
	memset(client_buff, 0, sizeof(client_buff));
	int size = 0;
	
	printf("Sending [%s] to the Server.\n", filename);

	//keep communicating with server
	while((size = fread(client_buff, sizeof(char), sizeof(client_buff), file_open))>0)
	{
		
		//printf("\nSending Data: %s\n", client_buff);
	 
		//Send some data
		if(send(client_sock , client_buff, size, 0) < 0)
		{
			printf("Send failed\n");
			return 1;
			//exit(1);
		}

		memset(client_buff, 0, sizeof(client_buff));
	}
	
	send(client_sock, "EOF", sizeof("EOF"), 0);


	printf("-------------------------------------------\n");
	printf("\tRESPONSE FROM SERVER\n");
	printf("-------------------------------------------\n\n");

	while(1)
	{
		memset(server_buff, 0, sizeof(server_buff));
		//Receive a reply from the server
		if(recv(client_sock , server_buff , 500 , 0) < 0)
		{
			printf("IO error");
			//break;
		}
 
		printf("Server sent: %s\n\n", server_buff);

		close(client_sock);
		fclose(file_open);
		return 0;
	}

}
