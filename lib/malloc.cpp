#include <string.h>
#include <system.h>

void* morecore(unsigned int);

struct mb
 {
 unsigned int size;
 bool free;
 mb* next;
 };

char mblock0[0x1000];
mb* first;

void init_malloc()
{
first = (mb*) mblock0;
first->size = 0x1000 - sizeof(mb);
first->next = 0;
first->free = true;
}

void* malloc(unsigned int size)
{
mb *i, *last;
for(i = first; i; i = i->next)
 {
 if(i->free)
  {
  if(i->size == size)
   {
   i->free = false;
   return (void*) ((unsigned int) i + sizeof(mb));
   }
  else if(i->size > size + sizeof(mb))
   {
   mb* n = (mb*) ((unsigned int) i + size + sizeof(mb));
   n->free = true;
   n->size = i->size - size - sizeof(mb);
   n->next = i->next;
   i->next = n;
   i->size = size;
   i->free = false;
   return (void*) ((unsigned int) i + sizeof(mb));
   }
  }
 if(i->next == NULL)
  last = i;
 }
unsigned int fullsize = bytes_to_pages(size) * 0x1000;
mb *na = (mb*) morecore(bytes_to_pages(size)), *nb;
if(na == NULL)
 return NULL;
na->size = size;
na->free = false;
nb = (mb*) ((unsigned int) na + sizeof(mb) + size);
nb->size = bytes_to_pages(size) * 0x1000 - size - 2 * sizeof(mb);
nb->free = true;
na->next = nb;
nb->next = NULL;
last->next = na;
return (void*) ((unsigned int) na + sizeof(mb));
}

void free(void* addr)
{
if(addr == NULL)
 return;
mb *i, *before = NULL;
for(i = first; i; i = i->next)
 {
 if((i->free == false) && ((unsigned int) i == (unsigned int) addr - sizeof(mb)))
  {
  i->free = true;
  if(i->next && i->next->free && ((unsigned int) i + i->size + sizeof(mb) == (unsigned int) i->next))
   {
   i->size += i->next->size + sizeof(mb);
   i->next = i->next->next;
   }
  if(before && before->free && ((unsigned int) before + before->size + sizeof(mb) == (unsigned int) i))
   {
   before->size += i->size + sizeof(mb);
   before->next = i->next;
   }
  return;
  }
 before = i;
 }
while(1);
}

void* calloc(unsigned int size, unsigned int count)
{
void* mem = malloc(size * count);
memset(mem, 0, size * count);
return mem;
}