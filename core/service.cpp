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

#include <service.h>
#include <stdlib.h>
#include <string.h>
#include <core.h>
#include <colors.h>
#include <stdio.h>
#include <hal.h>

ServiceManager::ServiceManager()
{
service = NULL;
index = 0;
}

unsigned int ServiceManager::load(unsigned int start, unsigned int length, char* cmdline, unsigned int restart_counter)
{
Service* s = new Service;
s->next = this->service;
this->service = s;
s->index = index++;
s->norestart = false;
s->restart_counter = restart_counter;

s->start = start;
s->length = length;
s->cmdline = cmdline;

return s->index;
}

void ServiceManager::process_kill(Task* task)
{
Service* s = find(task);
if(s)
 if(!s->norestart)
  {
  free((void*) s->r_start);
  if(--s->restart_counter != 0)
   {
   printf("Restarting crashed service '%s'... ", s->cmdline);
   s->r_task = start(s->index);
   if(s->r_task)
    printf("%zok%z\n", LIGHTGREEN, LIGHTGRAY);
   else
    printf("%zFAILED%z\n", LIGHTRED, LIGHTGRAY);
   }
  else
   {
   printf("%zToo many crashes of service '%s'. Not restarting.%z\n", LIGHTRED, s->cmdline, LIGHTGRAY);
   }
  }
}

Service* ServiceManager::find(unsigned int index)
{
Service* sv;
for(sv = service; sv; sv = sv->next)
 if(sv->index == index)
  return sv;
return NULL;
}

Service* ServiceManager::find(Task* t)
{
Service* sv;
for(sv = service; sv; sv = sv->next)
 if(sv->r_task == t)
  return sv;
return NULL;
}

Task* ServiceManager::start(unsigned int index)
{
Service* s = find(index);
s->r_start = (unsigned int) malloc(s->length);
memcpy((void*) s->r_start, (void*) s->start, s->length);
s->r_task = core->load_executable(s->start, s->length, s->cmdline);
s->r_task->pl = 1;
s->r_task->priority = 10;
s->r_task->tss->eflags |= 0x3000; // IOPL=3
return s->r_task;
}

void ServiceManager::stop(unsigned int index)
{
Service* s = find(index);
s->norestart = true;
hal->taskman->kill(s->r_task->index, 1);
free((void*) s->r_start);
s->norestart = false;
}