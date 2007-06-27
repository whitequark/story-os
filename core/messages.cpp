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
#include <core.h>
#include <assert.h>

unsigned int syscall_send(Registers r)
{
Task* receiver = hal->taskman->task(r.esi);
if(!receiver)
 return 1;
Message* msg = new Message;
msg->sender = hal->taskman->current->index;
msg->type = (MessageType) r.ebx;
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
 msg->prev = NULL;
 receiver->message->prev = msg;
 receiver->message = msg->next;
 }
else
 {
 msg->next = NULL;
 msg->prev = NULL;
 receiver->message = msg;
 receiver->message_pointer = receiver->message;
 }

if(receiver->reason == rsMessage)
 receiver->reason = rsNone;

hal->taskman->current->reason = rsReply;
asm("ljmp $0x30, $0");

return 0;
}

unsigned int syscall_check(Registers r)
{
return hal->taskman->current->message_pointer != NULL;
}

unsigned int syscall_length(Registers r)
{
return hal->taskman->current->message_pointer->length;
}

unsigned int syscall_type(Registers r)
{
return hal->taskman->current->message_pointer->type;
}

unsigned int syscall_sender(Registers r)
{
return hal->taskman->current->message_pointer->sender;
}

unsigned int syscall_data(Registers r)
{
memcpy((void*)r.ebx, hal->taskman->current->message_pointer->data, hal->taskman->current->message_pointer->length);
}

unsigned int syscall_wait(Registers r)
{
if(hal->taskman->current->message_pointer == NULL)
 {
 hal->taskman->current->reason = rsMessage;
 asm("ljmp $0x30, $0");
 }
}

unsigned int syscall_reply(Registers r)
{
Task* receiver = hal->taskman->task(hal->taskman->current->message->sender);
if(!receiver)
 return 1;

Message* msg = new Message;
msg->sender = hal->taskman->current->index;
msg->type = hal->taskman->current->message_pointer->type;
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
assert(receiver->reason == rsReply);
receiver->reason = rsNone;

if(hal->taskman->current->message_pointer->next)
 hal->taskman->current->message_pointer->next->prev = hal->taskman->current->message_pointer->prev;
if(hal->taskman->current->message_pointer->prev)
 hal->taskman->current->message_pointer->prev->next = hal->taskman->current->message_pointer->next;

if(hal->taskman->current->message_pointer->length != 0)
 free(hal->taskman->current->message_pointer->data);
delete hal->taskman->current->message_pointer;

bool change_message = false;
if(hal->taskman->current->message == hal->taskman->current->message_pointer)
 change_message = true;

if(hal->taskman->current->message_pointer->next == NULL)
 hal->taskman->current->message_pointer = hal->taskman->current->message_pointer->prev;
else
 hal->taskman->current->message_pointer = hal->taskman->current->message_pointer->next;

if(change_message)
 hal->taskman->current->message = hal->taskman->current->message_pointer;

return 0;
}

unsigned int syscall_reply_check(Registers r)
{
return hal->taskman->current->reply != NULL;
}

unsigned int syscall_reply_length(Registers r)
{
return hal->taskman->current->reply->length;
}

unsigned int syscall_reply_data(Registers r)
{
memcpy((void*) r.ebx, hal->taskman->current->reply->data, hal->taskman->current->reply->length);
return 0;
}

unsigned int syscall_reply_remove(Registers r)
{
free(hal->taskman->current->reply->data);
delete hal->taskman->current->reply;
hal->taskman->current->reply = NULL;
return 0;
}

unsigned int syscall_reset(Registers r)
{
hal->taskman->current->message_pointer = hal->taskman->current->message;
return 0;
}

unsigned int syscall_next(Registers r)
{
if(hal->taskman->current->message_pointer == NULL)
 return 0;
else if(hal->taskman->current->message_pointer->next == NULL)
 {
 hal->taskman->current->message_pointer = hal->taskman->current->message;
 return 1;
 }
else
 {
 hal->taskman->current->message_pointer = hal->taskman->current->message_pointer->next;
 return 0;
 }
}

Messenger::Messenger()
{
hal->syscalls->add(50, &syscall_check);
hal->syscalls->add(51, &syscall_send);
hal->syscalls->add(52, &syscall_next);
hal->syscalls->add(53, &syscall_reset);
hal->syscalls->add(54, &syscall_length);
hal->syscalls->add(55, &syscall_type);
hal->syscalls->add(56, &syscall_data);
hal->syscalls->add(57, &syscall_sender);
hal->syscalls->add(58, &syscall_wait);
hal->syscalls->add(59, &syscall_reply);
hal->syscalls->add(60, &syscall_reply_check);
hal->syscalls->add(61, &syscall_reply_length);
hal->syscalls->add(62, &syscall_reply_data);
hal->syscalls->add(63, &syscall_reply_remove);
}