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

extern "C" void _start()
{
die(main());
}

void* page_alloc(int count) //FIXME c++ maybe?
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
asm("int $0x30");
}

void delay(unsigned int millis)
{
asm("int $0x31"::"a"(4),"b"(millis));
asm("int $0x30");
}

void wait_irq(unsigned int irq)
{
asm("int $0x31"::"a"(3),"b"(irq));
asm("int $0x30");
}

void putchar(char c) //FIXME !
{
int tmp;
asm("int $0x31":"=a"(tmp):"a"(5),"b"(c));
}

void textcolor(char c) //FIXME !
{
int tmp;
asm("int $0x31":"=a"(tmp):"a"(6),"b"(c));
}