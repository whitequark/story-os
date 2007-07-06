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

#include <ipc.h>
#include <kprintf.h>
#include <hal.h>
#include <core.h>
#include <stdlib.h>
#include <procman.h>
#include <string.h>

void process_manager()
{
Messenger m;
while(1)
 {
 if(hal->taskman->current->message == NULL)
  {
  hal->taskman->current->reason = rsMessage;
  asm("int $0x31"::"a"(0)); //schedule next task
  }
 
 Message msg, reply;
 reply.size = 0;
 reply.buffer = NULL;
 reply.type = rtOk;
 msg.size = 0; //after receive() size = real message size
 msg.buffer = NULL;
 m.receive(msg);
 
 Task* task = hal->taskman->task(msg.task);
 
 switch(msg.type)
  {
  case Procman::mtDelay:
  unsigned int delay;
  msg.buffer = &delay;
  msg.size = sizeof(delay);
  m.receive(msg);
  task->reason = rsDelay;
  task->wait_object = delay;
  m.reply(reply);
  break;
  
  case Procman::mtDie:
  unsigned int return_code;
  msg.buffer = &return_code;
  msg.size = sizeof(return_code);
  m.receive(msg);
  hal->taskman->kill(msg.task, return_code);
  m.reply(reply);
  break;
  
  case Procman::mtWaitForDie:
  unsigned int tid;
  msg.buffer = &tid;
  msg.size = sizeof(tid);
  m.receive(msg);
  task->reason = rsTaskDie;
  task->wait_object = tid;
  m.reply(reply); //TODO make reply with return code
  break;
  
  case Procman::mtWaitForIRQ:
  unsigned int irq;
  msg.buffer = &irq;
  msg.size = sizeof(irq);
  m.receive(msg);
  task->reason = rsIRQ;
  task->wait_object = irq;
  m.reply(reply);
  break;
  
  case Procman::mtAllocPages:
  unsigned int count;
  msg.buffer = &count;
  msg.size = sizeof(count);
  m.receive(msg);
  
  void* address;
  address = task->vmm->alloc(count);
  reply.size = sizeof(address);
  reply.buffer = &address;
  m.reply(reply);
  break;
  
  case Procman::mtCreateThread:
  unsigned int entry_point;
  msg.buffer = &entry_point;
  msg.size = sizeof(entry_point);
  m.receive(msg);
  
  Task* t;
  t = hal->taskman->create_task(task->pl, entry_point, task->priority, task->vmm);
  if(t)
   task->vmm->change_threads(+1);
  
  unsigned int id;
  id = t ? t->index : 0;
  reply.size = sizeof(id);
  reply.buffer = &id;
  m.reply(reply);
  break;
  
  case 0xf:
  char s[1000];
  msg.buffer = &s;
  m.receive(msg);
  printf("%s", s);
  m.reply(reply);
  break;
  
  default:
  reply.type = rtError;
  m.reply(reply);
  break;
  }
 //hal->taskman->status();
 }
}