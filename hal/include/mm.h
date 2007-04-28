#ifndef _MM_H_
#define _MM_H_

typedef struct SMemoryBlock
{
unsigned int first;
unsigned int count;
SMemoryBlock* next;
} MemoryBlock;

class MemoryManager
{
private:
unsigned int page_bitmap[0x8000];
unsigned int count_pages;
unsigned int free_pages;
bool safe_printf;
MemoryManager();
void set_bit(unsigned int page);
bool get_bit(unsigned int page);
void reset_bit(unsigned int page);
MemoryBlock* mb;

public:
MemoryManager(unsigned int first_accessible_address, unsigned int memory);
void* alloc(unsigned int count);
void free(void* pointer);
unsigned int free_memory();
void set_safe_printf();
};

#endif