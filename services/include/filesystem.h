#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_

#include <msgtype.h>

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
FileID id;
File();

public:
enum { mtResolve };
File(char* name);
bool open();
};

#endif