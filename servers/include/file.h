#ifndef _FILE_H_
#define _FILE_H_

#include <ipc.h>

struct FileID
{
unsigned int filesystem_service;
unsigned int filesystem_id;
unsigned int device_service;
unsigned int device_id;
};

class File
{
private:
char* name;
unsigned int fs_server;
Messenger m;
FileID id;
File();

public:
enum { mtResolve, mtCreate };
File(char* name);
bool open();
bool create();
};

#endif