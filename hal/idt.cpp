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

#include <idt.h>
#include <string.h>
#include <hal.h>

struct IDTRegister
{
unsigned short limit;
unsigned int address;
} __attribute__((__packed__));

IDTRegister _idt_register;

IDT::IDT()
{
memset(&idt, 0, 256*8);
}

void IDT::set_trap(unsigned char number, void (*handler) (), unsigned short code_selector, unsigned char dpl)
{
char* selector = (char*) ((unsigned int)idt + number * 8);
selector[0] = ((unsigned int) handler & 0x000000FF);
selector[1] = ((unsigned int) handler & 0x0000FF00) >> 8;
selector[6] = ((unsigned int) handler & 0x00FF0000) >> 16;
selector[7] = ((unsigned int) handler & 0xFF000000) >> 24;

selector[2] = (code_selector & 0x00FF);
selector[3] = (code_selector & 0xFF00) >> 8;

selector[4] = 0;
selector[5] = 0x8f | (dpl & 0x3) << 5;
}

void IDT::set_interrupt(unsigned char number, void (*handler) (), unsigned short code_selector)
{
char* selector = (char*) ((unsigned int)idt + number * 8);
selector[0] = ((unsigned int) handler & 0x000000FF);
selector[1] = ((unsigned int) handler & 0x0000FF00) >> 8;
selector[6] = ((unsigned int) handler & 0x00FF0000) >> 16;
selector[7] = ((unsigned int) handler & 0xFF000000) >> 24;

selector[2] = (code_selector & 0x00FF);
selector[3] = (code_selector & 0xFF00) >> 8;

selector[4] = 0;
selector[5] = 0x8e;
}

void IDT::install()
{
_idt_register.limit = 256*8;
_idt_register.address = (unsigned int) &idt;
asm("lidt _idt_register");
}
