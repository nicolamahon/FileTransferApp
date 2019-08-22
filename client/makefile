CC = gcc
objects = client.o

prog			:	$(objects) 	
				$(CC) -o file_transfer_client $(objects)

client.o		:	client.c
				$(CC) -c client.c

clean			:	
				rm file_transfer_client $(objects)
