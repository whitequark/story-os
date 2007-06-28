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

#include <system.h>
#include <stdio.h>

int main();
void init_mallocator();

extern "C" void _start()
{
//init_mallocator();
die(main());
}

void* morecore(unsigned int count)
{
void* ret;
asm("int $0x31":"=a"(ret):"a"(7),"b"(count));
return ret;
}

void die(unsigned int return_code)
{
asm("int $0x31"::"a"(0),"b"(return_code));
}

void wait_die(unsigned int tid)
{
asm("int $0x31"::"a"(2),"b"(tid));
}

void delay(unsigned int millis)
{
asm("int $0x31"::"a"(4),"b"(millis));
}

void wait_irq(unsigned int irq)
{
asm("int $0x31"::"a"(3),"b"(irq));
}

bool map_pages(unsigned int physical_addr, unsigned int virtual_addr, unsigned int count)
{
bool ret;
asm("int $0x31":"=a"(ret):"a"(8),"b"(physical_addr),"c"(virtual_addr),"d"(count));
return ret;
}