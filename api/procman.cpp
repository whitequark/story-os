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

#include <procman.h>
#include <string.h>
#include <vsprintf.h>

Procman::Procman()
{
asm("int $0x31":"=a"(procman_tid):"a"(2));
Message msg;
msg.task = procman_tid;
msg.type = Procman::mtWaitForFilesystem;
msg.size = 0;
m.send(msg);
}

Procman::Procman(bool)
{
asm("int $0x31":"=a"(procman_tid):"a"(2));
}

void Procman::delay(unsigned int millis)
{
Message msg;
msg.task = procman_tid;
msg.type = Procman::mtDelay;
msg.size = sizeof(millis);
msg.buffer = &millis;
m.send(msg);

msg.size = 0;
m.receive_reply(msg);
}

void Procman::die(int code)
{
Message msg;
msg.task = procman_tid;
msg.type = Procman::mtDie;
msg.size = sizeof(code);
msg.buffer = &code;
m.send(msg);

msg.size = 0;
m.receive_reply(msg);
while(1);
}

int Procman::wait_for_die(unsigned int task)
{
Message msg;
msg.task = procman_tid;
msg.type = Procman::mtWaitForDie;
msg.size = sizeof(task);
msg.buffer = &task;
m.send(msg);

int value;
msg.size = sizeof(value);
msg.buffer = &value;
m.receive_reply(msg);

return value;
}

void Procman::attach_irq(unsigned int irq)
{
Message msg;
msg.task = procman_tid;
msg.type = Procman::mtAttachIRQ;
msg.size = sizeof(irq);
msg.buffer = &irq;
m.send(msg);

msg.size = 0;
m.receive_reply(msg);
}

void Procman::detach_irq(unsigned int irq)
{
Message msg;
msg.task = procman_tid;
msg.type = Procman::mtDetachIRQ;
msg.size = sizeof(irq);
msg.buffer = &irq;
m.send(msg);

msg.size = 0;
m.receive_reply(msg);
}

void* Procman::alloc_pages(unsigned int count)
{
Message msg;
msg.task = procman_tid;
msg.type = Procman::mtAllocPages;
msg.size = sizeof(count);
msg.buffer = &count;
m.send(msg);

void* value;
msg.size = sizeof(value);
msg.buffer = &value;
m.receive_reply(msg);

return value;
}

unsigned int Procman::create_thread(void* entry_point)
{
Message msg;
msg.task = procman_tid;
msg.type = Procman::mtCreateThread;
msg.size = sizeof(entry_point);
msg.buffer = &entry_point;
m.send(msg);

unsigned int value;
msg.size = sizeof(value);
msg.buffer = &value;
m.receive_reply(msg);

return value;
}

void Procman::wait_for_message()
{
Message msg;
msg.task = procman_tid;
msg.type = Procman::mtWaitForMessage;
msg.size = 0;
m.send(msg);

msg.size = 0;
m.receive_reply(msg);
}

void Procman::printf(char* fmt, ...)
{
char s[2000];
va_list list;
va_start(list, fmt);
vsprintf(s, fmt, list);
va_end(list);

Message msg;
msg.task = procman_tid;
msg.type = 0xf;
msg.size = 2000;
msg.buffer = s;
m.send(msg);

msg.size = 0;
m.receive_reply(msg);
}

unsigned int Procman::get_tid()
{
return procman_tid;
}

unsigned int Procman::get_fs_server_tid()
{
Message msg;
msg.task = procman_tid;
msg.type = Procman::mtGetFilesystemTID;
msg.size = 0;
m.send(msg);

unsigned int value;
msg.size = sizeof(value);
msg.buffer = &value;
m.receive_reply(msg);

return value;
}