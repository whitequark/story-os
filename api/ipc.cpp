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

Message::Message(MessageType type, unsigned int receiver, void* data, unsigned int length)
{
this->type = type;
this->receiver = receiver;
this->data = data;
this->length = length;
assert(length != 0);
}

Message::Message(void* data, unsigned int length)
{
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

bool Interface::add(char* fname, char* parameters, char* returnvalue)
{
unsigned int ret;
asm("int $0x31":"=a"(ret):"a"(102),"b"(name),"c"(fname),"d"(parameters),"S"(returnvalue));
return ret;
}

bool Interface::present(char* fname)
{
unsigned int ret;
asm("int $0x31":"=a"(ret):"a"(103),"b"(name),"c"(fname));
return ret;
}

bool Interface::present(char* fname, char* parameters)
{
unsigned int ret;
asm("int $0x31":"=a"(ret):"a"(104),"b"(name),"c"(fname),"d"(parameters));
return ret;
}

void Interface::require(char* fname)
{
require();
if(!present(fname))
 {
 printf("%zRequired function '%s' in '%s' not present. Terminating...%z\n", LIGHTRED, fname, name, LIGHTGRAY);
 die(1);
 }
}

void Interface::require(char* fname, char* parameters)
{
require();
if(!present(fname, parameters))
 {
 printf("%zRequired function '%s'('%s') in '%s' not present. Terminating...%z\n", LIGHTRED, fname, parameters, name, LIGHTGRAY);
 die(1);
 }
}

void Interface::wait(char* fname)
{
while(!present(fname));
}

void Interface::wait(char* fname, char* parameters)
{
while(!present(fname, parameters));
}

CallPacker::CallPacker(char* name, char* args)
{
this->name = name;
this->args = args;
length = strlen(name) + 1 + strlen(args) + 1;
for(int i = 0; i < strlen(args); i++)
 switch(args[i])
  {
  case 'b':
  length++;
  break;
  
  case 'w':
  length += 2;
  break;
  
  case 'd':
  length += 4;
  break;
  }
packed = (char*) morecore(1); //HACK malloc(length);
strcpy(packed, name);
strcpy((char*) ((unsigned int) packed + strlen(name)+1), args);

pointer = strlen((char*) packed) + 1 + strlen((char*) ((unsigned int) packed) + strlen(packed) + 1) + 1;
apointer = 0;
}

void CallPacker::dump()
{
printf("Name: '%s', arguments: '%s'\n", packed, ((unsigned int) packed) + strlen(packed) + 1);
printf("Data dump:\n");
unsigned int n = 0;
for(; n < length; n++)
 {
 printf("%x ", packed[n]);
 if(n == length-1)
  {
  int k;
  for(k = 0; k < (n / 16 + 1) * 16 - n - 1; k++)
   printf("   ");
  }
 if(n % 16 == 15 || n == length-1)
  {
  int k;
  printf(" |");
  for(k = n - 15; k < n; k++)
   if(packed[k] >= ' ' && packed[k] <= 0x80)
    printf("%c", packed[k]);
   else
    printf(".");
  printf("|");
  printf("\n");
  }
 }
}

bool CallPacker::push(char c)
{
if(args[apointer] != 'b')
 return false;
else
 {
 apointer++;
 *((char*)(((unsigned int)packed) + pointer)) = c;
 pointer++;
 }
}

bool CallPacker::push(int i)
{
if(args[apointer] != 'd')
 return false;
else
 {
 apointer++;
 *((int*)(((unsigned int)packed) + pointer)) = i;
 pointer += 4;
 }
}

bool CallPacker::push(short s)
{
if(args[apointer] != 'w')
 return false;
else
 {
 apointer++;
 *((short*)(((unsigned int)packed) + pointer)) = s;
 pointer += 2;
 }
}

void* CallPacker::data()
{
return (void*) packed;
}

unsigned int CallPacker::size()
{
return length;
}