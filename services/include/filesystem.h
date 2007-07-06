#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_

#include <list.h>

class FilesystemObject
{
private:
FilesystemObject(FilesystemObject&);
FilesystemObject& operator=(FilesystemObject&);

public:
FilesystemObject(char* name);
char* name;
List<FilesystemObject*>* children;
};

typedef List<FilesystemObject*> FSOList;

class Filesystem
{
private:
FSOList* files;
void show(int level, FSOList* list);
List<char*>* strip_path(char* path);

public:
Filesystem();
void show();
FilesystemObject* get(char* name);
bool add(char* name);
};

#endif