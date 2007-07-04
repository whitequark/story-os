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
#include <assert.h>
#include <string.h>

Paging::Paging()
{
int i,j;

hal->pagedir = (PageDirectory*) hal->mm->alloc(1);
memset(hal->pagedir, 0, 0x1000); //this will clear 'present' bit in whole directory

unsigned int page_tables = (unsigned int) hal->mm->alloc(32);

for(i = 0; i < 32; i++)
 {
 PageTable* table = (PageTable*) (page_tables + i * 0x1000);
 hal->pagedir->table[i] = (unsigned int) table | PAGE_PRESENT | PAGE_WRITABLE;
 
 for(j = (i == 0 ? 1 : 0); j < 0x400; j++)
  {
  unsigned int address = (i * 0x400 + j) << 12;
  table->page[j] = address | PAGE_PRESENT | PAGE_WRITABLE;
  }
 }
 
load_cr3(hal->pagedir);
}

unsigned int Paging::get_pte(PageDirectory* pagedir, unsigned int page)
{
PageTable* table = (PageTable*) (pagedir->table[page >> 10] & 0xFFFFF000);
if(table == NULL)
 {
 table = (PageTable*) hal->mm->alloc(1);
 memset(table, 0, 0x1000);
 pagedir->table[page >> 10] = (unsigned int) table | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
 }
return table->page[page & 0x3F];
}

void Paging::set_pte(PageDirectory* pagedir, unsigned int page, unsigned int value)
{
PageTable* table = (PageTable*) (pagedir->table[page >> 10] & 0xFFFFF000);
if(table == NULL)
 {
 table = (PageTable*) hal->mm->alloc(1);
 memset(table, 0, 0x1000);
 pagedir->table[page >> 10] = (unsigned int) table | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
 }
asm("invlpg (%0)"::"r" (page): "memory");
table->page[page & 0x3F] = value;
}

void Paging::load_cr3(PageDirectory* cr3)
{
asm("mov %0, %%eax \n mov %%eax, %%cr3"::"a"(cr3));
}

void Paging::enable()
{
asm("mov %%cr0, %%eax \n orl $0x80000000, %%eax \n mov %%eax, %%cr0":::"eax");
}

unsigned int Paging::bytes_to_pages(unsigned int bytes)
{
return (bytes % 0x1000 == 0) ? (bytes / 0x1000) : (bytes / 0x1000 + 1);
}