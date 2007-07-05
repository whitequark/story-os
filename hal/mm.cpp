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

#include <hal.h>
#include <string.h>

//high bit
#define PAGE_ALLOCATED	0x80000000
#define PAGE_FREE	0x00000000

MemoryManager::MemoryManager(unsigned int first_accessible_address, unsigned int memory_amount)
{
int i;
count_pages = memory_amount >> 12;
unsigned int reserved_pages = first_accessible_address >> 12;
free_pages = count_pages - reserved_pages;
mb = &first; //need to declare them statically 'cause no new or malloc at this point
mb->first = 0;
mb->count = reserved_pages;
mb->allocated = true;
mb->next = &second;
mb->next->first = reserved_pages + 1;
mb->next->count = free_pages;
mb->next->allocated = false;
mb->next->next = NULL;
show();
}

void* MemoryManager::alloc(unsigned int count, bool no_merge)
{
MemoryBlock *cmb, *nmb; //current memory block, next -//-, first -//- (lower)
for(cmb = mb; cmb != NULL; cmb = cmb->next)
 if(cmb->count >= count && cmb->allocated == false)
  break;
if(cmb == NULL)
 if(!no_merge)
  {
  merge();
  return alloc(count, true);
  }
 else return NULL;

nmb = cmb->next;
cmb->allocated = true;
if(cmb->count != count)
 {
 MemoryBlock* fmb = new MemoryBlock;
 fmb->count = cmb->count - count;
 fmb->first = cmb->first + count;
 fmb->next = nmb;
 cmb->count = count;
 cmb->next = fmb; //insert free fmb in the middle of vmb and nmb
 }
free_pages -= count;
return (void*) (cmb->first << 12);
}

void MemoryManager::free(void* address)
{
unsigned int page = ((unsigned int) address) >> 12;
MemoryBlock *cmb; //current memory block
for(cmb = mb; cmb != NULL; cmb = cmb->next)
 if(cmb->first == page)
  break;
if(cmb == NULL)
 return;
if(cmb->allocated == false)
 return;
cmb->allocated = false;
free_pages += cmb->count;
}

unsigned int MemoryManager::free_memory()
{
return free_pages << 12;
}

unsigned int MemoryManager::all_memory()
{
return count_pages << 12;
}

void MemoryManager::merge()
{
bool no_advance = false;
for(MemoryBlock* cmb = mb; cmb != NULL; cmb = no_advance ? cmb : cmb->next)
 if(!cmb->allocated && !cmb->next->allocated)
  {
  MemoryBlock* nnmb = cmb->next->next;
  cmb->count += cmb->next->count;
  delete cmb->next;
  cmb->next = nnmb;
  if(!cmb->next || cmb->next->allocated)
   no_advance = false;
  else
   no_advance = true;
  }
}

void MemoryManager::show()
{
MemoryBlock *cmb;
for(cmb = mb; cmb != NULL; cmb = cmb->next)
 printf("block at 0x%X: <-> 0x%X, allocated: %i\n", cmb->first << 12, cmb->count << 12, cmb->allocated);
}