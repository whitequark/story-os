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

VirtualMemoryManager::VirtualMemoryManager()
{
int i, j;
directory = (PageDirectory*) hal->mm->alloc(1);
memset(directory, 0, 0x1000); //this will clear 'present' bit in whole directory
memcpy(directory, hal->pagedir, 32 * 4);
threads = 1;
mb = new VMemoryBlock;
mb->first = 0;
mb->count = USER_SPACE_START - 1;
mb->allocated = true;
mb->reserved = true;
mb->description = "kernel area";
mb->next = new VMemoryBlock;
mb->next->first = USER_SPACE_START;
mb->next->count = 0xFFFFF - USER_SPACE_START;
mb->next->allocated = false;
mb->next->description = "free space";
mb->next->next = NULL;
}

VirtualMemoryManager::VirtualMemoryManager(bool) //for process manager
{
int i, j;
directory = (PageDirectory*) hal->mm->alloc(1);
memset(directory, 0, 0x1000); //this will clear 'present' bit in whole directory

unsigned int page_tables = (unsigned int) hal->mm->alloc(32);
for(i = 0; i < 32; i++)
 {
 PageTable* table = (PageTable*) (page_tables + i * 0x1000);
 directory->table[i] = (unsigned int) table | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
 
 for(j = (i == 0 ? 1 : 0); j < 0x400; j++)
  {
  unsigned int address = (i * 0x400 + j) << 12;
  table->page[j] = address | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
  }
 }

mb = new VMemoryBlock;
mb->first = 0;
mb->count = 0x10000 - 1;
mb->allocated = true;
mb->reserved = true;
mb->description = "kernel area";
mb->next = new VMemoryBlock;
mb->next->first = 0x10000;
mb->next->count = 0xFFFFF - 0x10000;
mb->next->allocated = false;
mb->next->description = "free space";
mb->next->next = NULL;

threads = 1;
}

void* VirtualMemoryManager::alloc(unsigned int count, char* descr, bool no_merge)
{
VMemoryBlock *vmb, *nmb; //virtual memory block, next -//-, first -//- (lower)
for(vmb = mb; vmb != NULL; vmb = vmb->next)
 if(vmb->count >= count && vmb->allocated == false)
  break;
if(vmb == NULL)
 if(!no_merge)
  {
  merge();
  return alloc(count, descr, true);
  }
 else return NULL;

void* phys = hal->mm->alloc(count);
if(phys == NULL)
 return NULL;

nmb = vmb->next;
vmb->allocated = true;
vmb->physical = phys;
if(vmb->count != count)
 {
 VMemoryBlock* fmb = new VMemoryBlock;
 fmb->count = vmb->count - count;
 fmb->first = vmb->first + count;
 fmb->next = nmb;
 fmb->description = vmb->description;
 vmb->count = count;
 vmb->next = fmb; //insert free fmb in the middle of vmb and nmb
 }
vmb->description = descr;
map((unsigned int) phys, vmb->first << 12, count, PAGE_USER | PAGE_WRITABLE | PAGE_PRESENT);
return (void*) (vmb->first << 12);
}

void VirtualMemoryManager::free(void* address)
{
unsigned int page = ((unsigned int) address) >> 12;
VMemoryBlock *vmb; //virtual memory block
for(vmb = mb; vmb != NULL; vmb = vmb->next)
 if(vmb->first == page)
  break;
if(vmb == NULL)
 return;
if(vmb->physical == NULL || vmb->allocated == false || vmb->reserved == true)
 return;
hal->mm->free(vmb->physical);
vmb->allocated = false;
vmb->physical = NULL;
}

void VirtualMemoryManager::load()
{
hal->paging->load_cr3(directory);
}

VirtualMemoryManager::~VirtualMemoryManager()
{
int i;
for(VMemoryBlock* vmb = mb; vmb != NULL; vmb = vmb->next)
 free((void*) (vmb->first << 12));
VMemoryBlock* nmb;
for(VMemoryBlock* vmb = mb; vmb != NULL; vmb = nmb)
 {
 nmb = vmb->next;
 delete vmb;
 }
for(int i = 33; i < 0x400; i++)
 if(directory->table[i])
  hal->mm->free((void*) directory->table[i]);
hal->mm->free(directory);
}

void VirtualMemoryManager::map(unsigned int phys, unsigned int virt, unsigned int count, unsigned int attr)
{
for(unsigned int i = 0; i < count; i++)
 hal->paging->set_pte(directory, (virt >> 12) + i, ((phys + (i << 12)) & 0xFFFFF000 ) | attr);
}

void VirtualMemoryManager::alloc_at(unsigned int phys, unsigned int virt, unsigned int count, unsigned int attr, bool reserved, char* description)
{
unsigned int pvirt = virt >> 12; //paged virtual
VMemoryBlock *vmb = NULL, *amb, *aamb, *nmb;//virtual memory block, allocated -//-, after allocated -//-, next -//-
for(VMemoryBlock* cmb = mb; cmb != NULL; cmb = cmb->next)
 if((cmb->first <= pvirt) && (cmb->first + cmb->count >= pvirt + count) && (cmb->allocated == false))
  vmb = cmb;
if(vmb == NULL)
 hal->panic("VMM::alloc_at: cannot find memory block fully including allocated block");
nmb = vmb->next;
if(vmb->first != pvirt)
 hal->panic("VMM::alloc_at: vmb->first != pvirt currently not supported");
else
 {
 amb = vmb;
 aamb = new VMemoryBlock;
 aamb->first = vmb->first + count;
 aamb->count = vmb->count - count;
 aamb->next = nmb;
 aamb->description = amb->description;
 }
amb->physical = (void*) phys;
amb->allocated = 1;
amb->reserved = reserved;
amb->count = count;
amb->next = aamb;
amb->description = description;
map(phys, virt, count, attr);
}

unsigned int VirtualMemoryManager::get_directory()
{
return (unsigned int)directory;
}

int VirtualMemoryManager::change_threads(int delta)
{
return (threads += delta);
}

void VirtualMemoryManager::show()
{
VMemoryBlock *vmb;
printf("    VMM status:\n");
for(vmb = mb; vmb != NULL; vmb = vmb->next)
 printf("      0x%X => 0x%X: <-> 0x%X, %c%c - %s\n", vmb->first << 12, vmb->physical, vmb->count << 12, vmb->allocated ? 'A' : '_', vmb->reserved ? 'R' : '_', vmb->description);
printf("\n");
}

void VirtualMemoryManager::merge()
{
bool no_advance = false;
//FIXME error! bochs when reset
return;
for(VMemoryBlock* vmb = mb; vmb != NULL; vmb = no_advance ? vmb : vmb->next)
 if(!vmb->allocated && !vmb->next->allocated)
  {
  VMemoryBlock* nnmb = vmb->next->next;
  vmb->count += vmb->next->count;
  delete vmb->next;
  vmb->next = nnmb;
  if(!vmb->next || vmb->next->allocated)
   no_advance = false;
  else
   no_advance = true;
  }
}