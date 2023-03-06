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

#include <gdt.h>
#include <colors.h>
#include <string.h>
#include <hal.h>

struct GDTRegister
{
unsigned short limit;
unsigned int address;
} __attribute__((__packed__));

GDTRegister _gdt_register;

int SystemCodeSeg;
int SystemDataSeg;

char* NullDescriptor::get_bytes()
{
return "\0\0\0\0\0\0\0\0";
}

SegmentDescriptor::SegmentDescriptor(unsigned int base, unsigned int limit, bool executable, bool writable, char dpl, bool op32bit, bool conforming)
{
bool gran4k = true;

bytes[0] = (limit & 0x000FF);
bytes[1] = (limit & 0x0FF00) >> 8;
bytes[6] = (limit & 0xF0000) >> 16;

bytes[2] = base & 0x000000FF;
bytes[3] = (base & 0x0000FF00) >> 8;
bytes[4] = (base & 0x00FF0000) >> 16;
bytes[7] = (base & 0xFF000000) >> 24;

bytes[5] = (dpl & 0x3) << 5;
bytes[5] |= executable << 3;
bytes[5] |= conforming << 2;
bytes[5] |= writable  << 1;
bytes[5] |= 0x80 | 0x10; // present application descriptor

bytes[6] |= (gran4k << 3 | op32bit << 2 ) << 4;
}

char* SegmentDescriptor::get_bytes()
{
return bytes;
}

TSSDescriptor::TSSDescriptor(unsigned int base)
{
bytes[0] = 0x67;
bytes[1] = 0;

bytes[2] = (base & 0x000000FF) >> 0;
bytes[3] = (base & 0x0000FF00) >> 8;
bytes[4] = (base & 0x00FF0000) >> 16;
bytes[7] = (base & 0xFF000000) >> 24;

bytes[5] = 0x89;
bytes[6] = 0;
}

char* TSSDescriptor::get_bytes()
{
return bytes;
}

GDT::GDT()
{
count = 0;
installed = false;
}

void GDT::show()
{
int i;
printf(" GDT:\n");
if(count > 0)
 for(i = 0; i < count; i++)
  printf("  Descriptor %i: %x %x %x %x %x %x %x %x\n", i, gdt[8*i+0], gdt[8*i+1], gdt[8*i+2], gdt[8*i+3], gdt[8*i+4], gdt[8*i+5], gdt[8*i+6], gdt[8*i+7]);
else
 printf("%zNo descriptors%z\n", LIGHTRED, LIGHTGRAY);
}

int GDT::add_descriptor(GDTDescriptor* desc)
{
char* bytes = desc->get_bytes();
memcpy((void*) ((unsigned int) gdt + 8*count), bytes, 8);
count++;
if(installed)
 install(); //see GDT::install()
return count - 1;
}

void GDT::modify_descriptor(GDTDescriptor* desc, unsigned short number)
{
char* bytes = desc->get_bytes();
memcpy((void*) ((unsigned int) gdt + 8*number), bytes, 8);
}

void GDT::install()
{
_gdt_register.limit = count * 8;
_gdt_register.address = (unsigned int) gdt;
asm("lgdt _gdt_register\n");

installed = true;
//If gdt is already installed, adding a descriptor MUST cause a reinstall(limit updating)
//But when we install system selectors, this will kill system
}

unsigned short GDT::make_segment(unsigned short number, char dpl)
{
return (dpl & 0x3) | (number << 3);
}
