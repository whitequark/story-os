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

#include <pic.h>
#include <hal.h>

PIC::PIC()
{
int i;
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

hal->outb(0x21, 0xFF);
hal->outb(0xA1, 0xFF);
}

void PIC::mask(char n)
{
unsigned char port = n > 7 ? 0xA1 : 0x21;
hal->outb(port, hal->inb(port) | (1 << (n % 8)));
}

void PIC::unmask(char n)
{
unsigned char port = n > 7 ? 0xA1 : 0x21;
hal->outb(port, hal->inb(port) & ~(1 << (n % 8)));
}
