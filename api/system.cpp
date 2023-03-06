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

File* stdin;
File* stdout;

extern "C" void _start()
{
init_malloc();
stdout = new File("/dev/stdout");
stdin = new File("/dev/stdin");
die(main());
}

void _thread_start(int(*func)())
{
die(func());
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
while(1);
}

unsigned int get_tid()
{
int tid;
RSYSCALL0(SYSCALL_GET_TID, tid);
return tid;
}

unsigned int start_thread(int(*address)())
{
Message m = {0};
m.receiver = PROCMAN_TID;
m.type = pcStartThread;
m.value1 = (unsigned int) &_thread_start;
m.value2 = (unsigned int) address;
send(m);
return m.value1;
}

void* attach_memory(unsigned int count, unsigned int physical)
{
Message m = {0};
m.receiver = PROCMAN_TID;
m.type = pcAttachMemory;
m.value1 = count;
m.value2 = physical;
send(m);
return (void*) m.value1;
}

unsigned int exec(char* path, char* parameters)
{
File prg(path);
if(prg.resolve() == frOk)
 {
 char* image = new char[prg.size()];
 prg.read(image, prg.size());
 int n;
 RSYSCALL2(3, n, prg.size(), image);
 return n;
 }
else
 return 0;
}

void wait_die(unsigned int tid)
{
Message m = {0};
m.receiver = PROCMAN_TID;
m.type = pcWaitDie;
m.value1 = tid;
send(m);
}

void printf(char* fmt, ...)
{
char buf[1024];
va_list list;
va_start(list, fmt);
vsprintf(buf, fmt, list);
va_end(list);
while(stdout->resolve() != frOk || stdout->is_mounted() == false);
stdout->write(buf, strlen(buf) + 1);
}

char getch()
{
char c;
while(stdin->resolve() != frOk || stdin->is_mounted() == false);
stdin->read(&c, 1);
return c;
}

int gain_io_privilegies()
{
Message m = {0};
m.receiver = PROCMAN_TID;
m.type = pcGainIOPrivilegies;
send(m);
return m.type;
}

void attach_irq(unsigned int irq)
{
Message m = {0};
m.receiver = PROCMAN_TID;
m.type = pcAttachIRQ;
m.value1 = irq;
send(m);
}

void delay(unsigned int delay)
{
Message m = {0};
m.receiver = PROCMAN_TID;
m.type = pcDelay;
m.value1 = delay;
send(m);
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
