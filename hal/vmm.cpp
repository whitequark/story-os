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
memcpy(directory, hal->pagedir, 512);
for(i = 0; i < 0x40000; i++)
 set_bit(i);
}

void* VirtualMemoryManager::alloc(unsigned int count)
{
int i;
for(i = 0x40000; i < 0x100000; i++)
 if(!get_bit(i))
  {
  int j;
  bool ok = true;
  for(j = i+1; j < i + count; j++)
   if(get_bit(i))
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
  set_bit(i);
  hal->paging->set_pte(directory, i, phys | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
  printf("/valloc:%i/", count);
  for(j = i+1; j < i+count; j++)
   {
   set_bit(j);
   hal->paging->set_pte(directory, j, (phys + (j << 12)) | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
   }
  return (void*) (i << 12);
  }
return NULL;
}

void VirtualMemoryManager::free(void* address)
{
return;
hal->panic("VFREE called!");
if(address == NULL)
 return;
unsigned int page = ((unsigned int) address) >> 12;
if(((unsigned int) address) & 0xFFF)
 hal->panic("Attempt to free non-aligned address %X\n", address);
if(!(page_bitmap[page] & PAGE_ALLOCATED))
 hal->panic("Attempt to free unallocated memory page: %X\n", address);
if(!(page_bitmap[page] & ~PAGE_ALLOCATED))
 hal->panic("Attempt to free zero-length memory block: %X\n", address);
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

VirtualMemoryManager::~VirtualMemoryManager()
{
hal->paging->load_cr3(hal->pagedir);
int i;
for(i = 0x40000; i < 0x100000; i++)
 if(get_bit(i))
  {
  #ifdef _DEBUGGING_VMM_
  printf("vmm: freeing %X = %X\n", i << 12, (page_bitmap[i] & ~PAGE_ALLOCATED) << 12);
  #endif
  free((void*) (i << 12));
  }
//TODO free tables
hal->mm->free(directory);
}

void VirtualMemoryManager::map(unsigned int phys, unsigned int virt, unsigned int count, unsigned int attr)
{
unsigned int i;
for(i = 0; i < count; i++)
 {
 hal->paging->set_pte(directory, (virt >> 12) + i, ((phys + (i << 12)) & 0xFFFFF000 ) | attr);
 set_bit(virt >> 12);
 #ifdef _DEBUGGING_VMM_
 printf("vmm: mapping 0x%X to 0x%X (flags 0x%x)\n", ((virt >> 12) + i) << 12, (phys + (i << 12)), attr);
 #endif
 }
}

unsigned int VirtualMemoryManager::get_directory()
{
return (unsigned int)directory;
}

inline void VirtualMemoryManager::set_bit(unsigned int page)
{
page_bitmap[page / 32] |= (1 << page % 32);
}

inline bool VirtualMemoryManager::get_bit(unsigned int page)
{
return (page_bitmap[page / 32] & (1 << page % 32)) != 0;
}

inline void VirtualMemoryManager::reset_bit(unsigned int page)
{
page_bitmap[page / 32] &= ~(1 << page % 32);
}