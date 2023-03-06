#ifndef _MM_H_
#define _MM_H_

struct MemoryBlock
{
unsigned int first;
unsigned int count;
bool allocated;
MemoryBlock* next;
};

class MemoryManager
{
private:
unsigned int count_pages;
unsigned int free_pages;
MemoryManager();
MemoryBlock* mb;
MemoryBlock first, second;

public:
MemoryManager(unsigned int first_accessible_address, unsigned int memory);
void* alloc(unsigned int count, bool no_merge = false);
void free(void* pointer);
unsigned int free_memory();
unsigned int all_memory();
void show();
void merge();
};

#endif
