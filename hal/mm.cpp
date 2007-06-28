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
for(i = 0; i < count_pages; i++)
 {
 if(i >= reserved_pages)
  reset_bit(i);
 else
  set_bit(i);  
 }
safe_printf = false;
}

void* MemoryManager::alloc(unsigned int count)
{
int i;
#ifdef _DEBUGGING_MM_
if(safe_printf)
 printf("/mm:+%i/", count);
#endif
unsigned int free_count = 0;
for(i = 0; i < count_pages; i++)
 {
 if(get_bit(i)) 
  free_count = 0;
 else
  free_count++;
 if(free_count == count)
  {
  int first_page = i - free_count + 1;
  set_bit(first_page);
  int n;
  for(n = 1; n < count; n++)
   set_bit(first_page + n);
  free_pages -= count;
  return (void*) (first_page << 12);
  }
 }
return NULL;
}

void MemoryManager::free(void* address)
{
return;
hal->panic("FREE called!");
if(address == NULL)
 return;
unsigned int page = ((unsigned int) address) >> 12;
if(((unsigned int) address) & 0xFFF)
 hal->panic("Attempt to free non-aligned address %X\n", address);
if(!(page_bitmap[page] & PAGE_ALLOCATED))
 hal->panic("Attempt to free unallocated memory page: %X\n", address);
if(!(page_bitmap[page] & ~PAGE_ALLOCATED))
 hal->panic("Attempt to free zero-length memory block: %X\n", address);
unsigned int i, count = page_bitmap[page] & ~PAGE_ALLOCATED;
#ifdef _DEBUGGING_MM_
printf("mm: freeing page %X: count %i\n", page, count);
#endif
for(i = page; i < page + count; i++)
 page_bitmap[i] = PAGE_FREE;
free_pages += count;
}

unsigned int MemoryManager::free_memory()
{
return free_pages << 12;
}

void MemoryManager::set_safe_printf()
{
safe_printf = true;
}

inline void MemoryManager::set_bit(unsigned int page)
{
page_bitmap[page / 32] |= (1 << page % 32);
}

inline bool MemoryManager::get_bit(unsigned int page)
{
return (page_bitmap[page / 32] & (1 << page % 32)) != 0;
}

inline void MemoryManager::reset_bit(unsigned int page)
{
page_bitmap[page / 32] &= ~(1 << page % 32);
}