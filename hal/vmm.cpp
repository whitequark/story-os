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
#include <vmm.h>
#include <string.h>
#include <stdio.h>

//high bit
#define PAGE_ALLOCATED	0x80000000
#define PAGE_MAPPED	0x40000000
#define PAGE_FREE	0x00000000

VirtualMemoryManager::VirtualMemoryManager()
{
int i, j;
directory = (PageDirectory*) hal->mm->alloc(1);
memset(directory, 0, 0x1000); //this will clear 'present' bit in whole directory

unsigned int page_tables = (unsigned int) hal->mm->alloc(0x200);
memset((void*) page_tables, 0, 0x1000*0x200); //this will clear 'present' bit in all tables

for(i = 0; i < 0x200 /* 0x7fffffff */; i++)
 {
 PageTable* table = (PageTable*) (page_tables + i * 0x1000);
 directory->table[i] = (unsigned int) table | PAGE_PRESENT | PAGE_WRITABLE;
 
 int count = (i == 0) ? 0x399 : 0x400;
 j = (i == 0) ? 1 : 0;
 for(; j < count; j++)
  {
  unsigned int address = (i * 0x400 + j) << 12;
  table->page[j] = address | PAGE_PRESENT | PAGE_WRITABLE;
  }
 }

for(i = 0; i < 0x80000; i++)
 page_bitmap[i] = PAGE_ALLOCATED;
}

void* VirtualMemoryManager::alloc(unsigned int count)
{
int i;
for(i = 0x80000; i < 0x100000; i++)
 if(page_bitmap[i] == PAGE_FREE)
  {
  int j;
  bool ok = true;
  for(j = i+1; j < i + count; j++)
   if(page_bitmap[i] != PAGE_FREE)
    {
    ok = false;
    break;
    }
  if(!ok)
   continue;
  #ifdef _DEBUGGING_VMM_
  printf("vmm: found free block at %X length %i\n", i,  count);
  #endif
  unsigned int phys = (unsigned int) hal->mm->alloc(count);
  page_bitmap[i] = PAGE_ALLOCATED | (phys >> 12);
  hal->paging->set_pte(directory, i, phys | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER); //TODO user mode?
  for(j = i+1; j < i+count; j++)
   {
   page_bitmap[j] = PAGE_ALLOCATED;
   hal->paging->set_pte(directory, j, (phys + (j << 12)) | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER); //TODO user mode?
   }
  return (void*) (i << 12);
  }
return NULL;
}

void VirtualMemoryManager::free(void* address)
{
if(address == NULL)
 return;
unsigned int page = ((unsigned int) address) >> 12;
if(((unsigned int) address) & 0xFFF)
 hal->panic("Attempt to free non-aligned address %X\n", address);
if(!(page_bitmap[page] & PAGE_ALLOCATED))
 hal->panic("Attempt to free unallocated memory page: %X\n", address);
if(!(page_bitmap[page] & ~PAGE_ALLOCATED))
 hal->panic("Attempt to free zero-length memory block: %X\n", address);
free(physical(address));
page_bitmap[page] = PAGE_ALLOCATED;
int i;
for(i = page; ; i++)
 if(page_bitmap[i] == PAGE_ALLOCATED)
  {
  hal->paging->set_pte(directory, i, 0); //unmap
  page_bitmap[i] = PAGE_FREE;
  }
 else
  break;
}

void VirtualMemoryManager::load()
{
hal->paging->load_cr3(directory);
}

void VirtualMemoryManager::show()
{
int i;
for(i = 0x80000; i < 0x100000; i++)
 if(page_bitmap[i] & PAGE_ALLOCATED)
  {
  if(page_bitmap[i] & (~PAGE_ALLOCATED))
   printf("vmm: block %X -> %X\n", i << 12, (page_bitmap[i] & (~PAGE_ALLOCATED)) << 12);
  }
 else if(page_bitmap[i] & PAGE_MAPPED)
  {
  if(page_bitmap[i] & (~PAGE_MAPPED))
   printf("vmm: block %X -> %X\n", i << 12, (page_bitmap[i] & (~PAGE_MAPPED)) << 12);
  }
}

void* VirtualMemoryManager::physical(void* virt)
{
void* phys = (void*) ((page_bitmap[(unsigned int)virt >> 12] & (~PAGE_ALLOCATED)) << 12);
#ifdef _DEBUGGING_VMM_
printf("vmm: physical address of %X = %X\n", virt, phys);
#endif
return phys;
}

VirtualMemoryManager::~VirtualMemoryManager()
{
int i;
for(i = 0; i < 0x100000; i++)
 if((page_bitmap[i] & ~PAGE_ALLOCATED) && !(page_bitmap[i] & PAGE_MAPPED))
  {
  #ifdef _DEBUGGING_VMM_
  printf("vmm: freeing %X = %X\n", i << 12, (page_bitmap[i] & ~PAGE_ALLOCATED) << 12);
  #endif
  hal->mm->free((void*) ((page_bitmap[i] & ~PAGE_ALLOCATED) << 12));
  }
hal->mm->free((void*) (directory->table[0] & 0xFFFFF000));
hal->mm->free(directory);
}

void VirtualMemoryManager::map(unsigned int phys, unsigned int virt, unsigned int count, unsigned int attr)
{
unsigned int i;
for(i = 0; i < count; i++)
 {
 hal->paging->set_pte(directory, (virt >> 12) + i, ((phys + (i << 12)) & 0xFFFFF000 ) | attr);
 page_bitmap[virt >> 12] = PAGE_MAPPED | (phys >> 12);
 #ifdef _DEBUGGING_VMM_
 printf("vmm: mapping 0x%X to 0x%X (flags 0x%x)\n", ((virt >> 12) + i) << 12, (phys + (i << 12)), attr);
 #endif
 }
}

unsigned int VirtualMemoryManager::get_directory()
{
return (unsigned int)directory;
}