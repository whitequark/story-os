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

#include <pic.h>
#include <hal.h>

PIC::PIC()
{
int i;
for(i = 0; i < 16; i++)
 mask(i);
}

void PIC::remap(char v1, char v2)
{
v1 &= 0xF8;
v2 &= 0xF8;
hal->outb(0x20, 0x11);
hal->outb(0xA0, 0x11);
hal->outb(0x21, v1);
hal->outb(0xA1, v2);
hal->outb(0x21, 0x04);
hal->outb(0xA1, 0x02);
hal->outb(0x21, 0x01);
hal->outb(0xA1, 0x01);
hal->outb(0x21, 0x0);
hal->outb(0xA1, 0x0);
}

void PIC::mask(char n)
{
if(n > 15)
 hal->panic("PIC::mask(): n > 16!");
if(n == 2)
 n = 9;
bool second_pic = n > 7;
if(second_pic)
 n -= 8;
char byte = 1 << n;
char port = second_pic ? 0xA1 : 0x21;
char mask = hal->inb(port);
mask = mask | byte;
hal->outb(port, mask);
}

void PIC::unmask(char n)
{
if(n > 15)
 hal->panic("PIC::unmask(): n > 16!");
bool second_pic = n > 7;
if(second_pic)
 n -= 8;
char byte = 1 << n;
char port = second_pic ? 0xA1 : 0x21;
char mask = hal->inb(port);
mask = mask & (~byte);
hal->outb(port, mask);
}