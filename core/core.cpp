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
#include <stdio.h>
#include <colors.h>
#include <string.h>
#include <hal.h>
#include <messages.h>

Core* core;

Core::Core(multiboot_info_t* mbi)
{
printf("%zInitializing CORE...%z ", GREEN, LIGHTGRAY);

//printf("Messenger... ");
messenger = new Messenger;
//printf("%zok%z\n", LIGHTGREEN, LIGHTGRAY);

printf("%zCOMPLETE%z\n\n", GREEN, LIGHTGRAY);

bool errors_found = false;

if(mbi->mods_count > 0)
 {
 printf("Loading modules (detected %i):\n", mbi->mods_count);
 
 module_t* mod;
 for(mod = (module_t*)mbi->mods_addr; mod->mod_start != NULL; mod++)
  {
  printf("Loading `%s'... ", mod->string);
  Task* module;
  if(module = load_executable(mod->mod_start, mod->mod_end - mod->mod_start, (char*) mod->string))
   printf("%zok%z (task %i)\n", LIGHTGREEN, LIGHTGRAY, module->index);
  else 
   {
   printf("%zFAILED%z\n", LIGHTRED, LIGHTGRAY);
   errors_found = true;
   continue;
   }
  module->pl = 1;
  module->priority = 30;
  }
 }
else
 hal->panic("No modules detected!\n");
if(errors_found)
 printf("%zErrors when loading modules!%z\n\n", RED, LIGHTGRAY);
printf("\n");
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
 printf("load_elf: program header element type 0x%X\n", p->p_type);
 #endif
 if(p->p_type == PT_NULL)
  ;
 else if(p->p_type == PT_LOAD)
  {
  unsigned int pages = hal->paging->bytes_to_pages(p->p_memsz);
  if(p->p_memsz == 0)
   pages = 1;
  #ifdef _DEBUGGING_ELF_LOADER_
  printf("load_elf: file 0x%X (%i bytes) -> virtual 0x%X (%i bytes)\n", p->p_offset, p->p_filesz, p->p_vaddr, p->p_memsz);
  printf("load_elf: flags = 0x%x, align = %i\n", p->p_flags, p->p_align);
  #endif
  //TODO make page attributes
  vmm->map(start + p->p_offset, p->p_vaddr, pages, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
  }
 else
  {
  #ifdef _DEBUGGING_ELF_LOADER_
  printf("load_elf: unknown section type\n");
  #endif
  }
 }
Task* task = hal->taskman->create_task(3, header->e_entry, 1, vmm);
return task;
}