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

if(receiver->message != NULL)
 {
 Message* last;
 for(last = receiver->message; last->next != 0; last = last->next);
 msg->next = NULL;
 last->next = msg;
 }
else
 {
 msg->next = NULL;
 receiver->message = msg;
 }

if(receiver->reason == rsMessage)
 receiver->reason = rsNone;
return 0;
}
unsigned int syscall_check(Registers r)
{
return hal->taskman->current->message != NULL;
}
unsigned int syscall_clear(Registers r)
{
Message* msg;
for(msg = hal->taskman->current->message; msg != NULL; msg = msg->next)
 {
 free(msg->data);
 delete msg;
 }
hal->taskman->current->message = NULL;
return 0;
}
unsigned int syscall_remove(Registers r)
{
Message* msg = hal->taskman->current->message;
hal->taskman->current->message = msg->next;
free(msg->data);
delete msg;
return 0;
}
unsigned int syscall_length(Registers r)
{
return hal->taskman->current->message->length;
}
unsigned int syscall_type(Registers r)
{
return hal->taskman->current->message->type;
}
unsigned int syscall_sender(Registers r)
{
return hal->taskman->current->message->sender;
}
unsigned int syscall_data(Registers r)
{
memcpy((void*)r.ebx, hal->taskman->current->message->data, hal->taskman->current->message->length);
}
unsigned int syscall_wait(Registers r)
{
if(hal->taskman->current->message == NULL)
 {
 hal->taskman->current->reason = rsMessage;
 asm("ljmp $0x30, $0"); //FIXME
 }
}

Messenger::Messenger()
{
hal->syscalls->add(50, &syscall_check);
hal->syscalls->add(51, &syscall_send);
hal->syscalls->add(52, &syscall_clear);
hal->syscalls->add(53, &syscall_remove);
hal->syscalls->add(54, &syscall_length);
hal->syscalls->add(55, &syscall_type);
hal->syscalls->add(56, &syscall_data);
hal->syscalls->add(57, &syscall_sender);
hal->syscalls->add(58, &syscall_wait);
}