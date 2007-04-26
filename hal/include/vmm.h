#ifndef _VMM_H_
#define _VMM_H_

#include <paging.h>

class VirtualMemoryManager
{
private:
PageDirectory* directory;
unsigned long page_bitmap[0x100000];

public:
VirtualMemoryManager();
~VirtualMemoryManager();
void* alloc(unsigned int count);
void free(void* address);
void* physical(void* virt);
void map(unsigned int phys, unsigned int virt, unsigned int count, unsigned int attr = PAGE_PRESENT | PAGE_WRITABLE);
void load();
void show();
unsigned int get_directory();
};

#endif