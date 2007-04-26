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
#include <assert.h>

Message::Message(MessageType type, unsigned int receiver, void* data, unsigned int length)
{
this->type = type;
this->receiver = receiver;
this->data = data;
this->length = length;
assert(length != 0);
}

bool Message::send()
{
bool ret;
asm("int $0x31":"=a"(ret):"a"(51),"b"(type),"c"(length),"d"(data),"S"(receiver));
return ret;
}

bool MessageQuery::pending()
{
bool ret;
asm("int $0x31":"=a"(ret):"a"(50));
return ret;
}