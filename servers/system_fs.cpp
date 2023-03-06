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

#include <system.h>
#include <vsprintf.h>
#include <string.h>

void init_malloc();
int main();

extern "C" void _start()
{
init_malloc();
die(main());
}

extern "C" void __gxx_personality_v0()
{
}

extern "C" void __cxa_pure_virtual()
{
}

void* morecore(unsigned int count)
{
Message m = {0};
m.receiver = PROCMAN_TID;
m.type = pcMorecore;
m.value1 = count;
send(m);
return (void*) m.value1;
}

void die(int code)
{
Message m = {0};
m.receiver = PROCMAN_TID;
m.type = pcDie;
m.value1 = code;
send(m);
}

unsigned int get_tid()
{
int tid;
RSYSCALL0(SYSCALL_GET_TID, tid);
return tid;
}

void printf(char* fmt, ...)
{
va_list list;
va_start(list, fmt);
char buf[1000] = {0};
vsprintf(buf, fmt, list);
va_end(list);
SYSCALL1(2, buf);
}

int send(Message& msg)
{
int ret;
RSYSCALL1(SYSCALL_SEND, ret, &msg);
return ret;
}

int receive(Message& msg)
{
int ret;
RSYSCALL1(SYSCALL_RECEIVE, ret, &msg);
return ret;
}

int reply(Message& msg)
{
int ret;
RSYSCALL1(SYSCALL_REPLY, ret, &msg);
return ret;
}

int forward(Message& msg)
{
int ret;
RSYSCALL1(SYSCALL_FORWARD, ret, &msg);
return ret;
}