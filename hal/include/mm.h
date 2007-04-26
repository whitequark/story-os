#ifndef _MM_H_
#define _MM_H_

class MemoryManager
{
private:
unsigned long page_bitmap[0x100000];
unsigned int count_pages;
unsigned int free_pages;
bool safe_printf;
MemoryManager();

public:
MemoryManager(unsigned int first_accessible_address, unsigned int memory);
void* alloc(unsigned int count);
void free(void* pointer);
unsigned int free_memory();
void set_safe_printf();
};

#endif