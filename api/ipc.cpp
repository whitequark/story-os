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
#include <stdlib.h>
#include <stdio.h>
#include <system.h>
#include <assert.h>
#include <colors.h>
#include <string.h>
#include <stdlib.h>

bool Messenger::send(Message msg)
{
bool ret;
asm("int $0x31":"=a"(ret):"a"(50),"b"(msg.type),"c"(msg.size),"d"(msg.buffer),"S"(msg.task));
return ret;
}

bool Messenger::receive(Message& msg)
{
bool ret;
asm("int $0x31":"=a"(ret):"a"(52),"b"(&msg));
return ret;
}

bool Messenger::reply(Message msg)
{
bool ret;
asm("int $0x31":"=a"(ret):"a"(51),"b"(msg.type),"c"(msg.size),"d"(msg.buffer));
return ret;
}

bool Messenger::receive_reply(Message& msg)
{
bool ret;
asm("int $0x31":"=a"(ret):"a"(53),"b"(&msg));
return ret;
}