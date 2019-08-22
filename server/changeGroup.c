#include <stdio.h>	// printf
#include <string.h>	// strcmp
#include <unistd.h>	// chown

void changeGroup(char *dest, char *filename, int uid)
{
	char *ugrp;		// for storing the group of the new document
	int ugrpID;		// int for setting group of new document
	char *ugrps;		// for storing the contents of the dest filepath from strtok

	// get the group name from the dest filepath
	char str[500];
	strcpy(str, dest);
	ugrps = strtok (str,"/");
	while (ugrps != NULL)
	{
		ugrp = ugrps ;
		ugrps = strtok (NULL, "/");
	}
	printf("GROUP: %s\n",ugrp); 

	// select the correct group for the document 
	if(strcmp(ugrp, "Sales") == 0)
	{
		ugrpID = 1005;
	}
	else if(strcmp(ugrp, "Marketing") == 0)
	{
		ugrpID = 1006;
	}
	else if(strcmp(ugrp, "Offers") == 0)
	{
		ugrpID = 1007;
	}
	else if(strcmp(ugrp, "Promotions") == 0)
	{
		ugrpID = 1008;
	}
	else if(strcmp(ugrp, "intranet") == 0)
	{
		ugrpID = 1009;	
	}


	// set the owner and group IDs of the new file at its final location
	char set_own[50] = "";
	strcpy(set_own, dest);
	strcat(set_own, "/");
	strcat(set_own, filename);
	if(chown(set_own, uid, ugrpID) == -1)
	{
		printf("Change Group Failed!\n");
	}

}
