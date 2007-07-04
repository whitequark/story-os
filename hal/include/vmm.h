#ifndef _VMM_H_
#define _VMM_H_

#include <paging.h>

#define USER_SPACE_START 0x10000 //in pages

typedef struct SVMemoryBlock
{
unsigned int first;
unsigned int count;
SVMemoryBlock* next;
} VMemoryBlock;

class VirtualMemoryManager
{
private:
PageDirectory* directory;
unsigned long page_bitmap[0x8000];
void set_bit(unsigned int page);
bool get_bit(unsigned int page);
void reset_bit(unsigned int page);

public:
VirtualMemoryManager();
VirtualMemoryManager(bool);
~VirtualMemoryManager();
void* alloc(unsigned int count);
void free(void* address);
void map(unsigned int phys, unsigned int virt, unsigned int count, unsigned int attr = PAGE_PRESENT | PAGE_WRITABLE);
void load();
unsigned int get_directory();
};

#endif