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

#include <application.h>
#include <ipc.h>

void Application::start()
{
die(run());
}

void Application::delay(unsigned int millis)
{
Messenger m;
Message msg;
msg.task = PROCMAN_TID;
msg.type = Procman::mtDelay;
msg.size = sizeof(millis);
msg.buffer = &millis;
m.send(msg);
}

void Application::die(int code)
{
Messenger m;
Message msg;
msg.task = PROCMAN_TID;
msg.type = Procman::mtDie;
msg.size = sizeof(code);
msg.buffer = &code;
m.send(msg);
}

int Application::wait_for_die(unsigned int task)
{
Messenger m;
Message msg;
msg.task = PROCMAN_TID;
msg.type = Procman::mtWaitForDie;
msg.size = sizeof(task);
msg.buffer = &task;
m.send(msg);
}

int Application::wait_for_irq(unsigned int irq)
{
Messenger m;
Message msg;
msg.task = PROCMAN_TID;
msg.type = Procman::mtWaitForIRQ;
msg.size = sizeof(irq);
msg.buffer = &irq;
m.send(msg);
}

void* Application::alloc_pages(unsigned int count)
{
Messenger m;
Message msg;
msg.task = PROCMAN_TID;
msg.type = Procman::mtAllocPages;
msg.size = sizeof(count);
msg.buffer = &count;
m.send(msg);
}