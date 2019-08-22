#ifndef server_H_
#define server_H_

void *acceptClient(void *client);
void changeGroup(char *dest, char *filename, int uid);
void transfer(void *client, char *temp_file, char *dest);

#endif // server_H_
