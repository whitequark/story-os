//    This file is part of the Story OS
//    Copyright (C) 2007  Peter Zotov
//
//    Story OS is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    Story OS is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. 

#include <assert.h>
#include <story.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <hal.h>
#include <system.h>

void* grow(int npages);

#define LAST_LINK -1

typedef struct mb_st
 {
 size_t size;
 struct mb_st *link;
 } mb_t;

static int avail;
static mb_t avlmb;

void init_mallocator()
{
mb_t *cmb;
  
avlmb.size = 0;
avlmb.link = (mb_t*) page_alloc(1);
  
cmb = avlmb.link;
cmb->size = PAGE_SIZE;
cmb->link = (mb_t*)LAST_LINK;
}

void *malloc (size_t size)
{
void *allocated;
mb_t *p, *q, *t, *cmb, *tmb;
size_t k;
size_t needsize;
unsigned int begin_block, end_block, begin_block_size, new_block_size, new_block_begin;

assert(size != 0);
needsize = sizeof(mb_t) + size;
 
q = &avlmb;

while(1)
 {
 begin_block = (unsigned int)q->link; 
 if(begin_block == LAST_LINK)
  {
  q->link = (mb_t*) grow( mem_to_pages(size) );
  begin_block = (unsigned int)q->link;
  }
 t = (mb_t*)begin_block;

 if(t->size > needsize + 8)
  {
  begin_block_size = t->size;
  
  end_block = begin_block + begin_block_size;
  new_block_size = needsize;
  new_block_begin = end_block - new_block_size;
  allocated = (void*) (new_block_begin + sizeof(mb_t));
    
  p = (mb_t*)new_block_begin;
  p->size = new_block_size;
  t->size -= new_block_size;

  if(t->size == 0)
   q->link = t->link;
   break;
  } 
 else
  q = (mb_t*) begin_block;
 }
tmb = (mb_t *) ( (unsigned int) allocated - sizeof(mb_t));
return allocated;
}

void* calloc (size_t num, size_t size)
{
void* s = malloc(num * size);
memset(s, 0, num * size);
return s;
}

void* grow(int npages)
{
mb_t *mb;
mb = (mb_t*) page_alloc(npages);
mb->size = PAGE_SIZE * npages;
mb->link = (mb_t*) LAST_LINK;
return mb;
}

void *realloc (void *ptr, size_t size)
{
void* new_block;
mb_t *mb;

mb = (mb_t*) ((unsigned int)ptr - sizeof(mb_t));
if(mb->size == 0) 
 {
 printf("ATTEMPT TO REALLOC ZERO-LENGTH MEMORY BLOCK. PTR: %X\n", ptr);
 for(;;);
 }

new_block = malloc(size);
memcpy(new_block, ptr, size);
free(ptr);
return new_block;
}
 
void free (void *ptr)
{
mb_t *mb, *q, *p;
void *alink;
 
mb = (mb_t*) ((unsigned int)ptr - sizeof(mb_t));
if(mb->size == 0)
 return;

p = avlmb.link;
while(1)
 {
 if( ((unsigned int) p < (unsigned int) mb ) &&
     (((unsigned int) p->link > (unsigned int) mb) ||
     p->link == (void*)LAST_LINK))
  {
 
  //   [ A ]   [ B ]   [ C ] 
  //B is linked to C by default
  mb->link = p->link;

  //trying to merge B and C
  if(mb->link != (void*)LAST_LINK) 
   { 
   //does C exist?
   if( ((unsigned int) mb + mb->size) == (unsigned int)mb->link) 
    {
    mb->size += mb->link->size;
    mb->link = mb->link->link;
    }
   }
  if( ((unsigned int) p + p->size) == (unsigned int)mb) 
   {
   //setting A->link = B->link
   //and A->size = A->size + B->size
   p->size += mb->size;
   p->link = mb->link;
   break;
   }
  p->link = mb;
  break;
  } 
 else 
  p = p->link;
 }
}

