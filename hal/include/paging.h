#ifndef _PAGING_H_
#define _PAGING_H_

#include <mutex.h>

#define PAGE_PRESENT 0x1
#define PAGE_WRITABLE 0x2
#define PAGE_USER 0x4

typedef unsigned int PageFrame;

typedef struct SPageDirectory
{
PageFrame table[1024];
} PageDirectory;

typedef struct SPageTable
{
PageFrame page[1024];
} PageTable;

class Paging
{
private:
Mutex page_mutex;

public:
Paging();
void enable();

void load_cr3(PageDirectory* cr3);

void set_pte(PageDirectory* pagedir, unsigned int page, unsigned int value);
unsigned int get_pte(PageDirectory* pagedir, unsigned int page);
};

#endif
