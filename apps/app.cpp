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

#include <ipc.h>
#include <procman.h>
#include <mutex.h>

Mutex mutex;

void thread()
{
Procman p;
while(1)
 {
 char c = '2';
 
 Messenger m;
 Message msg;
 msg.task = PROCMAN_TID;

 msg.type = 0xf;
 msg.size = 1;
 msg.buffer = &c;
 m.send(msg);
 p.delay(500);
 }
}

int main()
{
Procman p;
p.create_thread((void*) &thread);
while(1)
 {
 char c = '1';
 
 Messenger m;
 Message msg;
 msg.task = PROCMAN_TID;

 msg.type = 0xf;
 msg.size = 1;
 msg.buffer = &c;
 m.send(msg);
 p.delay(1000);
 }
}