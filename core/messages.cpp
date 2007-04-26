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
#include <messages.h>
#include <story.h>
#include <stdio.h>
#include <string.h>

extern "C"
 {
 unsigned int syscall_send(Registers r)
  {
  Task* receiver = hal->taskman->task(r.esi);
  if(!receiver)
   return 1;
  Message* msg = new Message;
  msg->sender = hal->taskman->current->index;
  msg->type = (MessageType) r.ebx;
  msg->length = r.ecx;
  msg->data = malloc(msg->length);
  memcpy(msg->data, (void*) r.edx, msg->length);
  msg->next = receiver->message;
  receiver->message = msg;
  return 0;
  }
 unsigned int syscall_check(Registers r)
  {
  return hal->taskman->current->message != NULL;
  }
 }

Messenger::Messenger()
{
hal->syscalls->add(50, &syscall_check);
hal->syscalls->add(51, &syscall_send);
}