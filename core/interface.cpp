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
#include <stdio.h>
#include <string.h>

unsigned int syscall_iregister(Registers r)
{
return core->interfaces->add((char*)r.ebx);
}

unsigned int syscall_icheck(Registers r)
{
return core->interfaces->get((char*)r.ebx) != NULL;
}

unsigned int syscall_fregister(Registers r)
{
return core->interfaces->function_add((char*)r.ebx, (char*) r.ecx, (char*) r.edx);
}

unsigned int syscall_fcheck(Registers r)
{
return core->interfaces->function_get((char*)r.ebx, (char*) r.ecx) != NULL;
}

unsigned int syscall_fcheck_parameters(Registers r)
{
return core->interfaces->function_get((char*)r.ebx, (char*) r.ecx, (char*) r.edx) != NULL;
}

InterfaceManager::InterfaceManager()
{
interface = NULL;
hal->syscalls->add(100, &syscall_iregister);
hal->syscalls->add(101, &syscall_icheck);
hal->syscalls->add(102, &syscall_fregister);
hal->syscalls->add(103, &syscall_fcheck);
hal->syscalls->add(104, &syscall_fcheck_parameters);
}

Interface* InterfaceManager::get(char* name)
{
Interface* intf;
for(intf = interface; intf; intf = intf->next)
 if(!strcmp(intf->name, name))
  return intf;
return NULL;
}

Function* InterfaceManager::function_get(char* iname, char* fname)
{
Interface* intf = get(iname);
if(!intf)
 return NULL;
Function* f;
for(f = intf->function; f; f = f->next)
 if(!strcmp(f->name, fname))
  return f;
return NULL;
}

Function* InterfaceManager::function_get(char* iname, char* fname, char* parameters)
{
Interface* intf = get(iname);
if(!intf)
 return NULL;
Function* f;
for(f = intf->function; f; f = f->next)
 if(!strcmp(f->name, fname) && !strcmp(f->parameters, parameters))
  return f;
return NULL;
}

bool InterfaceManager::function_add(char* iname, char* fname, char* parameters)
{
Interface* intf = get(iname);
if(!intf)
 {
 #ifdef _DEBUGGING_INTERFACE_
 printf("Cannot register function '%s': no interface '%s'\n", fname, iname);
 #endif
 return 1;
 }
if(!function_get(iname, fname))
 {
 Function* func = new Function;
 func->name = strdup(fname);
 func->next = intf->function;
 func->parameters = strdup(parameters);
 intf->function = func;
 #ifdef _DEBUGGING_INTERFACE_
 printf("Registered function '%s' in interface '%s'\n", fname, iname);
 #endif
 return 0;
 }
else
 {
 #ifdef _DEBUGGING_INTERFACE_
 printf("Cannot register function '%s' in interface '%s'\n", fname, iname);
 #endif
 return 1;
 }
}

bool InterfaceManager::add(char* name)
{
if(!get(name))
 {
 Interface* intf = new Interface;
 intf->name = strdup(name);
 intf->task = hal->taskman->current->index;
 intf->next = interface;
 intf->function = NULL;
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
Interface* intf;
if(interface != NULL)
 for(intf = interface; intf; intf = intf->next)
  if(intf->task == task->index)
   {
   Interface* n = intf->next;
   Interface* p = intf->prev;
   Function* f;
   for(f = intf->function; f; f = f->next) 
    {
    free(f->name);
    free(f->parameters);
    delete f;
    }
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