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

#include <core.h>
#include <colors.h>
#include <string.h>
#include <hal.h>
#include <messages.h>
#include <vmm.h>
#include <procman.h>

Core* core;

Core::Core(multiboot_info_t* mbi)
{
printf("%zInitializing CORE...%z ", GREEN, LIGHTGRAY);

for(int i = 0; i < 16; i++)
 IRQs[i] = NULL;
messenger = new CoreMessenger;
launch_procman();

printf("%zCOMPLETE%z\n", GREEN, LIGHTGRAY);

bool errors_found = false;

if(mbi->mods_count > 0)
 {
 printf("%zLoading modules%z:\n", GREEN, LIGHTGRAY);
 
 module_t* mod;
 for(mod = (module_t*)mbi->mods_addr; mod->mod_start != NULL; mod++)
  {
  printf("Loading `%s'... ", mod->string);
  Task* t = core->load_executable(mod->mod_start, mod->mod_end - mod->mod_start, (char*) mod->string);
  if(t)
   {
   t->pl = 1;
   t->priority = 10;
   t->tss->eflags |= 0x3000; // IOPL=3
   printf("%zok%z (task %i)\n", LIGHTGREEN, LIGHTGRAY, t->index);
   }
  else 
   {
   printf("%zFAILED%z\n", LIGHTRED, LIGHTGRAY);
   errors_found = true;
   continue;
   }
  }
 }
else hal->panic("No modules detected!\n");

if(errors_found) printf("%zErrors when loading modules!%z\n", RED, LIGHTGRAY);
else             printf("%zLoaded successfully%z\n", GREEN, LIGHTGRAY);
 
printf("Free memory: %i KB / %i KB\n\n", 
	hal->mm->free_memory() / 0x400, hal->mm->all_memory() / 0x400);
}

Task* Core::load_executable(unsigned int start, unsigned int size, char* command_line)
{
unsigned char* file = (unsigned char*) start;
if(file[0] == 0x7f && file[1] == 'E' && file[2] == 'L' && file[3] == 'F')
 {
 #ifdef _DEBUGGING_EXECUTABLE_LOADER_
 printf("\nload_executable: ELF detected\n");
 #endif
 return load_elf(start, size);
 }
else if(file[0] == 'M' && file[1] == 'Z')
 {
 printf("\nload_executable: PE not supported\n");
 return NULL;
 }
#ifdef _DEBUGGING_EXECUTABLE_LOADER_
printf("\nload_executable: wrong file format\n");
#endif
return NULL;
}

Task* Core::load_elf(unsigned int start, unsigned int size)
{
const char* phdr_types[] = { "NULL", "LOAD", "DYNAMIC", "INTERP", "NOTE", "SHLIB", "PHDR" };

Elf32_Ehdr* header = (Elf32_Ehdr*) start;
if(header->e_type != ET_EXEC || header->e_machine != EM_386)
 {
 #ifdef _DEBUGGING_ELF_LOADER_
 printf("load_elf: wrong machine or type\n");
 #endif
 return NULL;
 }

#ifdef _DEBUGGING_ELF_LOADER_
printf("load_elf: entry point is 0x%X\n", header->e_entry);
#endif

VirtualMemoryManager* vmm = new VirtualMemoryManager;

Elf32_Phdr* pheader = (Elf32_Phdr*) (start + header->e_phoff);
Elf32_Phdr* p;
for(p = pheader; p < pheader + header->e_phnum; p++)
 {
 #ifdef _DEBUGGING_ELF_LOADER_
 printf("load_elf: program header element type 0x%x\n", p->p_type);
 #endif
 if(p->p_type == PT_NULL)
  ;
 else if(p->p_type == PT_LOAD)
  {
  unsigned int pages = hal->paging->bytes_to_pages(p->p_memsz);
  if(p->p_filesz == 0) //FIXME: some bug at alloc_at???
   continue;
  unsigned int phys = (start + p->p_offset) & 0xFFFFF000;
  unsigned int virt = p->p_vaddr & 0xFFFFF000;
  unsigned int count = pages;
  unsigned int flags = PAGE_PRESENT | PAGE_USER | (p->p_flags & PF_W ? PAGE_WRITABLE : 0);
  #ifdef _DEBUGGING_ELF_LOADER_
  printf("load_elf: file 0x%X (0x%x bytes) -> virtual 0x%X (0x%x bytes)\n", 
  	p->p_offset, p->p_filesz, p->p_vaddr, p->p_memsz);
  printf("load_elf: flags = 0x%x, align = 0x%x\n", p->p_flags, p->p_align);
  printf("load_elf: mapping 0x%X to 0x%X: %i pages, %s\n", phys, virt, count,
  	flags & PAGE_WRITABLE ? "writable" : "read only");
  #endif
  vmm->alloc_at(phys, virt, count, flags, true, "ELF PT_LOAD segment");
  }
 else
  {
  #ifdef _DEBUGGING_ELF_LOADER_
  printf("load_elf: unknown section type\n");
  #endif
  }
 }
Task* task = hal->taskman->create_task(3, header->e_entry, 1, vmm);
task->image = (void*) start;
return task;
}

void Core::launch_procman()
{
VirtualMemoryManager* vmm = new VirtualMemoryManager(true);
procman = hal->taskman->create_task(0, (unsigned int) &process_manager, 1, vmm);
procman->tss->eflags |= 0x3000;
procman_initialized = false;
}

void Core::process_irq(unsigned int irq)
{
if(irq == 7)
 return;
List<unsigned int>* i;
if(IRQs[irq])
 iterate_list(i, IRQs[irq])
  {
  Message msg;
  msg.type = Procman::mtIRQFired;
  msg.task = i->item;
  msg.buffer = &irq;
  msg.size = sizeof(irq);
  messenger->send(0, &msg);
  }
}

void Core::attach_irq(unsigned int irq, unsigned int task)
{
if(irq > 15)
 return;
if(IRQs[irq] == NULL)
 IRQs[irq] = new List<unsigned int>(task);
else
 IRQs[irq]->add_tail(new List<unsigned int>(task));
}