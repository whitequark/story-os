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
#include <string.h>
#include <core.h>
#include <assert.h>

unsigned int syscall_send(Registers r)
{
Task* receiver = hal->taskman->task(r.esi);
if(!receiver)
 return 1;
 
hal->cli();
CoreMessage* msg = new CoreMessage;
msg->sender = hal->taskman->current->index;
msg->type = r.ebx;
msg->length = r.ecx;
if(r.ecx != 0)
 {
 msg->data = malloc(msg->length);
 memcpy(msg->data, (void*) r.edx, msg->length);
 }
else
 msg->data = NULL;

if(receiver->message != NULL)
 {
 msg->next = receiver->message;
 receiver->message = msg;
 }
else
 {
 msg->next = NULL;
 receiver->message = msg;
 }

if(receiver->reason == rsMessage)
 receiver->reason = rsNone;

hal->taskman->current->reason = rsReply;

hal->sti();
hal->taskman->schedule();

return 0;
}

unsigned int syscall_reply(Registers r)
{
Task* receiver = hal->taskman->task(hal->taskman->current->message->sender);
if(!receiver)
 return 1;

hal->cli();
CoreMessage* msg = new CoreMessage;
msg->sender = hal->taskman->current->index;
msg->type = r.ebx;
msg->length = r.ecx;
if(r.ecx != 0)
 {
 msg->data = malloc(msg->length);
 memcpy(msg->data, (void*) r.edx, msg->length);
 }
else
 msg->data = NULL;

if(receiver->reply != NULL)
 {
 if(receiver->reply->length != 0)
  free(receiver->reply->data);
 delete receiver->reply;
 }

receiver->reply = msg;
if(receiver->reason == rsReply)
 receiver->reason = rsNone;

CoreMessage* next = hal->taskman->current->message->next;

if(hal->taskman->current->message->length != 0)
 free(hal->taskman->current->message->data);
delete hal->taskman->current->message;

hal->taskman->current->message = next;
hal->sti();

return 0;
}

unsigned int syscall_receive(Registers r)
{
if(hal->taskman->current->message == NULL)
 return 1;
Message* msg = (Message*) r.ebx;
msg->type = hal->taskman->current->message->type;
msg->task = hal->taskman->current->message->sender;
unsigned int length = 
	msg->size < hal->taskman->current->message->length ? msg->size : hal->taskman->current->message->length;
memcpy(msg->buffer, hal->taskman->current->message->data, length);
msg->size = hal->taskman->current->message->length;
return 0;
}

CoreMessenger::CoreMessenger()
{
hal->syscalls->add(50, &syscall_send);
hal->syscalls->add(51, &syscall_reply);
hal->syscalls->add(52, &syscall_receive);
}