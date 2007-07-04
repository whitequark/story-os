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

#include <interface.h>
#include <task.h>
#include <hal.h>
#include <core.h>
#include <string.h>

unsigned int syscall_iregister(Registers r)
{
return core->interfaces->add((char*)r.ebx);
}

unsigned int syscall_icheck(Registers r)
{
return core->interfaces->get((char*)r.ebx) != NULL;
}

unsigned int syscall_igettask(Registers r)
{
if(core->interfaces->get((char*)r.ebx) == NULL)
 return 0;
return core->interfaces->get((char*)r.ebx)->task;
}

InterfaceManager::InterfaceManager()
{
interface = NULL;
hal->syscalls->add(100, &syscall_iregister);
hal->syscalls->add(101, &syscall_icheck);
hal->syscalls->add(102, &syscall_igettask);
}

CoreInterface* InterfaceManager::get(char* name)
{
CoreInterface* intf;
for(intf = interface; intf; intf = intf->next)
 if(!strcmp(intf->name, name))
  return intf;
return NULL;
}

bool InterfaceManager::add(char* name)
{
if(!get(name))
 {
 CoreInterface* intf = new CoreInterface;
 intf->name = strdup(name);
 intf->task = hal->taskman->current->index;
 intf->next = interface;
 if(interface)
  interface->prev = intf;
 intf->prev = NULL;
 interface = intf;
 #ifdef _DEBUGGING_INTERFACE_
 printf("Registered interface '%s'\n", name);
 #endif
 return 0;
 }
else
 {
 #ifdef _DEBUGGING_INTERFACE_
 printf("Cannot register interface '%s'\n", name);
 #endif
 return 1;
 }
}

void InterfaceManager::process_kill(Task* task)
{
CoreInterface* intf;
if(interface != NULL)
 for(intf = interface; intf; intf = intf->next)
  if(intf->task == task->index)
   {
   CoreInterface* n = intf->next;
   CoreInterface* p = intf->prev;
   free(intf->name);
   delete intf;
   if(interface == intf)
    interface = n;
   if(n)
    n->prev = p;
   if(p)
    p->next = n;
   }
}