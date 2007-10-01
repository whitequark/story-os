#ifndef _FILE_H_
#define _FILE_H_

#define MAX_PATH 1024

/*
filesystem server must fill up value1 in resolve message with tid of read/write task

All: frCommandNotSupported, frPathTooLong, frOk
foCreate: 
foResolve: frFileNotFound
foMount: frFileNotFound frAlreadyMounted
foUnmount: frFileNotFound
foRead: 
foWrite: 

Query format:
foCreate  (client): d = path
foCreate   (reply): v1 = file id
foResolve (client): d = path
foResolve (server): d = path part, v2 = mount_parameter, v2 <= 1
foResolve  (reply): v1 = file id, v2 = is mounted
foMount   (client): d = path, v1 = tid, v2 = parameter
(foUnmount)
foWrite   (client): d = data, v1 = file_id, (v2 = position)
foRead    (client): v1 = file id, (v2 = position)
foRead     (reply): d = data
*/

typedef enum { foCreate, foResolve, foMount, foUnmount, foRead, foWrite } FSOperation;
typedef enum { frOk, frCommandNotSupported, frPathTooLong, frFileNotFound, frAlreadyMounted };

class File
{
private:
static unsigned int root_fs_server;
char* name;
unsigned int server_tid;
unsigned int file_id;
bool mounted;

int get_root_fs();

File();

public:
File(char* name);
int create();
int resolve();
int mount(unsigned int tid, unsigned int parameter = 0);
int write(void* data, unsigned int length);
int read(void* data, unsigned int length);
bool is_mounted();
};

#endif