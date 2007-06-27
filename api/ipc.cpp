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

Message::Message(unsigned int type, unsigned int receiver, void* data, unsigned int length)
{
this->type = (MessageType) type;
this->receiver = receiver;
this->data = data;
this->length = length;
}

Message::Message(void* data, unsigned int length)
{
this->data = data;
this->length = length;
}

bool Message::send()
{
bool ret;
asm("int $0x31":"=a"(ret):"a"(51),"b"(type),"c"(length),"d"(data),"S"(receiver));
return ret;
}

bool Message::reply()
{
bool ret;
asm("int $0x31":"=a"(ret):"a"(59),"c"(length),"d"(data));
return ret;
}

bool MessageQuery::pending()
{
bool ret;
asm("int $0x31":"=a"(ret):"a"(50));
return ret;
}

MessageType MessageQuery::type()
{
MessageType ret;
asm("int $0x31":"=a"(ret):"a"(55));
return ret;
}

unsigned int MessageQuery::length()
{
unsigned int ret;
asm("int $0x31":"=a"(ret):"a"(54));
return ret;
}

void MessageQuery::data(void* data)
{
asm("int $0x31"::"a"(56),"b"(data));
}

void MessageQuery::wait()
{
asm("int $0x31"::"a"(58));
}

void* MessageQuery::alloc_data()
{
void* ret = morecore(1); //HACK malloc(length());
data(ret);
return ret;
}

unsigned int MessageQuery::sender()
{
unsigned int ret;
asm("int $0x31":"=a"(ret):"a"(57));
return ret;
}

bool MessageQuery::next()
{
unsigned int ret;
asm("int $0x31":"=a"(ret):"a"(52));
return ret;
}

unsigned int Reply::length()
{
unsigned int ret;
asm("int $0x31":"=a"(ret):"a"(61));
return ret;
}

bool Reply::check()
{
unsigned int ret;
asm("int $0x31":"=a"(ret):"a"(60));
return ret;
}

void Reply::data(void* data)
{
asm("int $0x31"::"a"(62),"b"(data));
}
 
void Reply::remove()
{
asm("int $0x31"::"a"(63));
}

Interface::Interface(char* name)
{
this->name = name;
}

bool Interface::add()
{
unsigned int ret;
asm("int $0x31":"=a"(ret):"a"(100),"b"(name));
return ret;
}

bool Interface::present()
{
unsigned int ret;
asm("int $0x31":"=a"(ret):"a"(101),"b"(name));
return ret;
}

void Interface::require()
{
if(!present())
 {
 printf("%zRequired interface '%s' not present. Terminating...%z\n", LIGHTRED, name, LIGHTGRAY);
 die(1);
 }
}

void Interface::wait()
{
while(!present());
}

unsigned int Interface::task()
{
unsigned int ret;
asm("int $0x31":"=a"(ret):"a"(102),"b"(name));
return ret;
}
