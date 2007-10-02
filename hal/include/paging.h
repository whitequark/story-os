#ifndef _PAGING_H_
#define _PAGING_H_

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
public:
Paging();
void enable();

void load_cr3(PageDirectory* cr3);

void set_pte(PageDirectory* pagedir, unsigned int page, unsigned int value);
unsigned int get_pte(PageDirectory* pagedir, unsigned int page);
};

inline unsigned int bytes_to_pages(unsigned int bytes)
 { return (bytes % 0x1000 == 0) ? (bytes / 0x1000) : (bytes / 0x1000 + 1); }


#endif