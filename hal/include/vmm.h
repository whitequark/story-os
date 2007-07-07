#ifndef _VMM_H_
#define _VMM_H_

#include <paging.h>

#define USER_SPACE_START 0x10000 //in pages

struct VMemoryBlock
{
unsigned int first;
unsigned int count;
void* physical;
bool allocated;
bool reserved;
char* description;
VMemoryBlock* next;
};

class VirtualMemoryManager
{
private:
PageDirectory* directory;
VMemoryBlock* mb;
int threads;
void merge();

public:
VirtualMemoryManager();
VirtualMemoryManager(bool);
~VirtualMemoryManager();
void* alloc(unsigned int count, char* descr = "", bool no_merge = false);
void free(void* address);
void map(unsigned int phys, unsigned int virt, unsigned int count, unsigned int attr);
void alloc_at(unsigned int phys, unsigned int virt, unsigned int count, unsigned int attr, bool reserved, char* description = "");
void load();
unsigned int get_directory();
int change_threads(int delta);
void show();
};

#endif