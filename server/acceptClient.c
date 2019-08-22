#include <stdio.h>      // for IO
#include <string.h>     //for strlen
#include <sys/socket.h> // for socket
#include <unistd.h>     //for write
#include <stdlib.h> 	// for malloc
#include <grp.h>	// getgrouplist
#include <pthread.h>	// threads
#include <sys/wait.h>	// wait
#include <pwd.h>	// username, passwd

#include "server.h"

void *acceptClient(void *client)
{

	// passed reference from main for client socket
	int client_sock;
	client_sock = *(int *)client;

	// variables
	int READSIZE = 0;  	// size of sockaddr_in for client connection
	char file[500];		// buffer for user file to be transferred
	char filename[50];	// string for user filename
	char dest[500];		// string for the file destination
	char temp_file[100] = "/var/www/tmp/";	// string for location of the temp file
	
	int uid;		// user id received from client
	gid_t gid;		// group id recieved from client

	// initilaise mutex lock
	pthread_mutex_t lock_x;

	// create the mutex lock
	pthread_mutex_init(&lock_x, NULL);

	// check the current IDs
	printf("[Server] Effective User ID: %d\n", geteuid());
	printf("[Server] Effective Group ID: %d\n", getegid());

	printf("\n-------------------------------------------\n");
	printf("\tRECEIVING DATA FROM CLIENT\n");
	printf("-------------------------------------------\n\n");

	// clear buffers for new data
	memset(filename, 0, sizeof(filename)); 	// name of file being transferred
	memset(dest, 0, sizeof(dest));		// user's chosen destination 
	memset(file, 0, sizeof(file));	 	// buffer for reading file

	/* START RECEIVING DATA FROM CLIENT SOCKET */
	READSIZE = recv(client_sock , filename , sizeof(filename) , 0);
	printf("Filename: %s\n", filename);
	
	READSIZE = recv(client_sock , dest , sizeof(dest) , 0);
	printf("Destination: %s\n", dest);	

	READSIZE = recv(client_sock , &uid , sizeof(uid) , 0);
	printf("[Client] User ID: %d\n", uid);

	READSIZE = recv(client_sock , &gid , sizeof(gid) , 0);
	printf("[Client] Group ID: %d\n", gid);

	// get username from the UID
	struct passwd *pwd;
	pwd = getpwuid(uid);
	printf("Username: %s\n", pwd->pw_name);

	// create the full file path for the temp file
	strcat(temp_file, filename);

	// lock resources 
	pthread_mutex_lock(&lock_x);

	// open temp file to write user's file
	FILE *tmp_file = fopen(temp_file, "w");	

	/* RECEIVE FILE FROM CLIENT SOCKET */
	while((READSIZE = recv(client_sock , file , sizeof(file), 0)) > 0 )
	{
		// get the last 3 chars of the message send over the client socket - EOF msg
		char *eof = &file[strlen(file)-3];

		// set size of data to write to the file minus the EOF msg
		char data[strlen(file)-3];

		// copy (file less the EOF_msg) to a new buffer to write to file
		strncpy(data, file, strlen(file)-3);
		
		// if the client has indicated this is the end of the message
		if(strcmp(eof, "EOF") == 0)		
		{
			// write to the open file as normal
			fwrite(data, sizeof(char), strlen(data), tmp_file);
		
			// clear the buffer
			memset(file, 0, sizeof(file));
			
			// break out of the while loop
			break;
		}

		// if not EOF, write as normal
		fwrite(file, sizeof(char), READSIZE, tmp_file);

		// clear the buffer
		memset(file, 0, sizeof(file));
	}

	// close the tmp file
	fclose(tmp_file);

	/* GET THE USER'S GROUPS */
	int rootID = 0;
	int i, ngroups;
	gid_t *groups;	// variable to store possible groups user belongs to
	ngroups = 20;	
	// large enough to prevent returning -1 from getgrouplist, 
	// which requires a resize of the returned list to accomodate 
	// a return size greater than ngroups
	
	// dynamically assign memory for the groups array
	groups = malloc(ngroups * sizeof(gid_t));	

	// try to get the user's groups
	if(getgrouplist(pwd->pw_name, uid, groups, &ngroups) == -1)
	{
		printf("ERROR: getting group list...\n");
	}
	else
	{
		// display the group values - more for testing purposes
		printf("User Groups: \n[  ");
		for(i = 0; i < ngroups; i++)
		{
			printf("%d  ", groups[i]);
		}
		printf("]\n");
	}

	/* CHANGE FROM ROOT TO USER */
	setgroups(ngroups, groups);

	// set the effective IDs
	setegid(uid);
	seteuid(uid);

	// check change has been applied
	printf("[Server] Effective User ID: %d\n", geteuid());
	printf("[Server] Effective Group ID: %d\n", getegid());

	/* BEGIN TRANSFER OF FILE: tmp_file to dest */
	transfer((void*)&client_sock, temp_file, dest);


	// client has closed the connection
	if(READSIZE == 0)
	{
		puts("Client disconnected\n");
		fflush(stdout);
	}
	// there was an error reading from the client socket
	else if(READSIZE == -1)
	{
		perror("read error");
	}

	/* CHANGE BACK TO ROOT USER */
	seteuid(rootID);
	setegid(rootID);

	// check change has been applied
	printf("[Server] Effective User ID: %d\n", geteuid());
	printf("[Server] Effective Group ID: %d\n", getegid());

	// release lock on resources
	pthread_mutex_unlock(&lock_x);

	// release memory allocated using malloc
	free(groups);

	// call external function to change group for the new file
	changeGroup(dest, filename, uid);

	printf("\n-------------------------------------------\n");
	printf("WAITING FOR INCOMING CONNECTION FROM CLIENT\n");
	printf("-------------------------------------------\n\n");


}


