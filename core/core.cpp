//    This file is part of the Story OS
//    Copyright (C) 2007  Catherine 'whitequark'
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
#include <vmm.h>
#include <messages.h>

Core* core;
unsigned int exec_syscall(Registers r);

Core::Core(multiboot_info_t* mbi)
{
printf("%zInitializing CORE...%z ", GREEN, LIGHTGRAY);

messenger = new Messenger;
launch_procman();
hal->syscalls->add(3, &exec_syscall);

printf("%zCOMPLETE%z\n", GREEN, LIGHTGRAY);

bool errors_found = false;

if(mbi->mods_count > 0)
 {
 printf("%zLoading modules%z:\n", GREEN, LIGHTGRAY);
 
 module_t* mod;
 for(mod = (module_t*)mbi->mods_addr; mod->mod_start != NULL; mod++)
  {
  if(!strcmp((char*) mod->string, "/fs") || !strcmp((char*) mod->string, "/init"))
   {
   printf("Loading `%s'... ", mod->string);
   Task* t = core->load_executable(mod->mod_start, mod->mod_end - mod->mod_start, (char*) mod->string);
   if(t)
    {
    t->pl = 1;
    t->priority = 1;
    t->tss->eflags |= 0x3000; // IOPL=3
    t->wait_reason = wrNone;
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
 }
else hal->panic("No modules detected!\n");

if(errors_found) printf("%zErrors when loading modules!%z\n", RED, LIGHTGRAY);
else             printf("%zLoaded successfully%z\n", GREEN, LIGHTGRAY);
 
printf("Free memory: %i KB / %i KB\n", 
	hal->mm->free_memory() / 0x400, hal->mm->all_memory() / 0x400);
}

Task* Core::load_executable(unsigned int start, unsigned int size, char* command_line)
{
unsigned char* file = (unsigned char*) start;
if(file[0] == 0x7f && file[1] == 'E' && file[2] == 'L' && file[3] == 'F')
 return load_elf(start, size);
else if(file[0] == 'M' && file[1] == 'Z')
 {
 printf("\nload_executable: PE not supported\n");
 return NULL;
 }
printf("\nload_executable: wrong file format\n");
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
  unsigned int pages = bytes_to_pages(p->p_memsz);
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

unsigned int exec_syscall(Registers r)
{
void* image = hal->mm->alloc(bytes_to_pages(r.ebx));
memcpy(image, (void*) r.ecx, r.ebx);
Task* task = core->load_executable((unsigned int) image, r.ebx, NULL);
if(!task)
 return 0;
else
 {
 task->tss->eflags |= 0x3000;
 task->wait_reason = wrNone;
 return task->index;
 }
}

void process_manager()
{
unsigned int fs_server_tid = 0;
while(1)
 {
 char data[MAX_PATH];
 Message msg = {0};
 msg.data = data;
 msg.data_length = MAX_PATH;
 receive(msg);
 
 Task* sender = hal->taskman->task(msg.sender);
 Task* n;
 
 switch(msg.type)
  {
  case pcMorecore:
  msg.value1 = (unsigned int) sender->vmm->alloc(msg.value1, "morecore");
  msg.type = prOk;
  break;
  
  case pcDie:
  hal->taskman->kill(msg.sender, msg.value1);
  msg.type = prOk;
  break;
  
  case pcSetRootFS:
  if(fs_server_tid == 0)
   {
   fs_server_tid = msg.sender;
   printf("procman: setting root FS server to %d\n", msg.sender);
   msg.type = prOk;
   }
  break;
  
  case pcGetRootFS:
  msg.value1 = fs_server_tid;
  msg.type = prOk;
  break;
  
  case pcStartThread:
  n = hal->taskman->create_task(3, msg.value1, 1, sender->vmm, 1, &msg.value2);
  sender->vmm->change_threads(+1);
  n->tss->eflags |= 0x3000;
  n->wait_reason = wrNone;
  msg.value1 = n->index;
  msg.type = prOk;
  break;
  
  case pcGainIOPrivilegies:
  sender->tss->eflags |= 0x3000;
  msg.type = prOk;
  break;
  
  case pcAttachMemory:
  msg.value1 = (unsigned int) sender->vmm->map_new_virtual(msg.value1, msg.value2, "AttachMemory request");
  msg.type = prOk;
  break;
  
  case pcAttachIRQ:
  core->attach_irq(msg.value1, msg.sender);
  msg.type = prOk;
  break;
  
  case pcDelay:
  msg.type = prOk;
  reply(msg);
  sender->wait_reason = wrDelay;
  sender->wait_object = msg.value1;
  continue;
  
  case pcWaitDie:
  if(hal->taskman->task(msg.value1)->wait_reason != wrDead)
   {
   sender->wait_reason = wrTaskDie;
   sender->wait_object = msg.value1;
   }
  break;
  
  //MODULES SERVER
  case foResolve:
  module_t* mod;
  int i;
  for(mod = (module_t*)hal->mbi->mods_addr, i = 0; mod->mod_start != NULL; mod++, i++)
   if(!strcmp((char*) (mod->string + 1), (char*) msg.data))
    break;
  if(mod->mod_start == NULL)
   msg.type = frFileNotFound;
  else
   {
   msg.value1 = i;
   msg.value2 = 1;
   msg.type = frOk;
   }
  break;
  
  case foRead:
  for(mod = (module_t*)hal->mbi->mods_addr, i = 0; mod->mod_start != NULL; mod++, i++)
   if(i == msg.value1)
    break;
  if(mod == NULL)
   msg.type = frFileNotFound;
  else
   {
   msg.reply = new char[msg.reply_length];
   memcpy(msg.reply, (void*) (mod->mod_start + msg.value2), msg.reply_length);
   msg.type = frOk;
   }
  break;
  
  case foGetAttributes:
  for(mod = (module_t*)hal->mbi->mods_addr, i = 0; mod->mod_start != NULL; mod++, i++)
   if(i == msg.value1)
    break;
  if(mod == NULL)
   msg.type = frFileNotFound;
  else
   {
   msg.value1 = mod->mod_end - mod->mod_start;
   msg.type = frOk;
   }
  break;
  
  default:
  msg.type = frCommandNotSupported;
  }
 reply(msg);
 }
}

void Core::launch_procman()
{
VirtualMemoryManager* vmm = new VirtualMemoryManager(true);
for(int i = 0; i < 16; i++)
 IRQs[i] = NULL;
procman = hal->taskman->create_task(0, (unsigned int) &process_manager, 1, vmm);
procman->tss->eflags |= 0x3000;
procman->wait_reason = wrNone;
}

void Core::process_irq(unsigned int irq)
{
List<unsigned int>* i;
if(IRQs[irq])
 {
 iterate_list(i, IRQs[irq])
  {
  Message *m = new Message;
  m->sender = 0;
  m->type = pmIRQFired;
  m->value1 = irq;
  Task* tsk = hal->taskman->task(i->item);
  if(tsk->messages)
   tsk->messages->add_tail(new List<Message*>(m));
  else
   tsk->messages = new List<Message*>(m);
  }
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
