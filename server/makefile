CC = gcc
objects = server.o acceptClient.o changeGroup.o transfer.o
headers = server.h	

prog			:	$(objects) 	
				$(CC) -o file_transfer $(objects) -lpthread

server.o		:	server.c $(headers)
				$(CC) -c server.c

acceptClient.o		:	acceptClient.c
				$(CC) -c acceptClient.c -lpthread

changeGroup.o		:	changeGroup.c
				$(CC) -c changeGroup.c

transfer.o		:	transfer.c
				$(CC) -c transfer.c

clean			:	
				rm file_transfer $(objects)
