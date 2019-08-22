#include <string.h>	// strlen
#include <unistd.h>	// execlp
#include <sys/wait.h>	// wait
#include <stdlib.h>	// exit_status
#include <stdio.h>	// printf


void transfer(void *client, char *temp_file, char *dest)
{

	// passed reference from main for client socket
	int client_sock;
	client_sock = *(int *)client;

	// child process ID
	pid_t pid;

	// fork and check for error
	if((pid = fork()) == -1)
	{
		printf("ERROR CREATING CHILD....\n");
		exit(EXIT_FAILURE);
	}
	// child process
	else if(pid == 0)
	{
		// copy the src file (live) as a new file in the backups folder
		execlp("cp", "cp", "-p", temp_file, dest, NULL);
	
		// any code here will not run if the execlp call succeeds
		printf("ERROR COPYING FILE TO NEW DESTINATION....\n");
		exit(EXIT_FAILURE);
		
	}
	// parent process
	else 
	{
		// parent waits for child process to finish
		int status=0;
		wait(&status);	

		// check the return status of the child
		if(WIFEXITED(status))
		{
			int exit_status = WEXITSTATUS(status);
			
			// write a message to the client to confirm success or failure
			if(exit_status > 0)
			{
				printf("COPY FAILED\n");
				write(client_sock , "COPY FAILED" , strlen("COPY FAILED"));
			}
			else
			{
				printf("COPY SUCCESS\n");
				write(client_sock , "COPY SUCCESS" , strlen("COPY SUCCESS"));
			}
		}	
	}	
}
