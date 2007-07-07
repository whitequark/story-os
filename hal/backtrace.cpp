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

//alghorytm based on OSKit FreeBSD glue code - stack_trace.c

#include <hal.h>
#include <elf.h>
#include <multiboot.h>
#include <string.h>
#include <kprintf.h>
#include <colors.h>

struct _symtab
{
Elf32_Sym* symbols;
unsigned int strings;
unsigned int length;
};

struct _function
{
char* name;
unsigned int address;
};

_symtab null = { NULL, 0, 0 };

multiboot_info_t* get_multiboot_info()
{
return hal->mbi;
}

void* get_user_elf_image()
{
return hal->taskman->current->image;
}

_symtab get_user_symtab()
{
_symtab s;
s.length = 0;

Elf32_Ehdr* header = (Elf32_Ehdr*) get_user_elf_image();
Elf32_Sym* symbols;

if(header == NULL)
 return s;

Elf32_Shdr* hdr = (Elf32_Shdr*) ((unsigned int) get_user_elf_image() + (unsigned int) header->e_shoff);
Elf32_Shdr* symtab = NULL;
Elf32_Shdr* shstrtab = &hdr[header->e_shstrndx]; //it says where is .shstrtab
Elf32_Shdr* strtab = NULL;

for(int i = 0; i < header->e_shnum; i++) //find .symtab
 if(hdr[i].sh_type == SHT_SYMTAB)
  symtab = &hdr[i];
if(!symtab)
 return s;

for(int i = 0; i < header->e_shnum; i++) //find .strtab
 if(hdr[i].sh_type == SHT_STRTAB && strcmp((char*) ((unsigned int) get_user_elf_image() + shstrtab->sh_offset + hdr[i].sh_name), ".strtab") == 0)
  strtab = &hdr[i];
if(!strtab)
 return s;

s.strings = (unsigned int) get_user_elf_image() + strtab->sh_offset;
s.symbols = (Elf32_Sym*) ((unsigned int) get_user_elf_image() + symtab->sh_offset); //symbol table
s.length = symtab->sh_size / sizeof(Elf32_Sym);

return s;
}

_symtab get_kernel_symtab()
{
multiboot_info_t* mbi = get_multiboot_info();
Elf32_Sym* symbols;

Elf32_Shdr* hdr = (Elf32_Shdr*) mbi->u.elf_sec.addr;
Elf32_Shdr* symtab = NULL;
Elf32_Shdr* shstrtab = &hdr[mbi->u.elf_sec.shndx]; //grub says where is .shstrtab
Elf32_Shdr* strtab = NULL;

for(int i = 0; i < mbi->u.elf_sec.num; i++) //find .symtab
 if(hdr[i].sh_type == SHT_SYMTAB)
  symtab = &hdr[i];

for(int i = 0; i < mbi->u.elf_sec.num; i++) //find .strtab
 if(hdr[i].sh_type == SHT_STRTAB && strcmp((char*) (shstrtab->sh_addr + hdr[i].sh_name), ".strtab") == 0)
  strtab = &hdr[i];

_symtab s;
s.strings = strtab->sh_addr;
s.symbols = (Elf32_Sym*) symtab->sh_addr; //symbol table
s.length = symtab->sh_size / sizeof(Elf32_Sym);

return s;
}

_function get_function(_symtab symtab, unsigned int address)
{
_function f;
f.name = NULL;
f.address = 0;
for(int i = 0; i < symtab.length; i++)
 {
 unsigned char type = symtab.symbols[i].st_info & 0xf;
 if(type == STT_SECTION || type == STT_NOTYPE || type == STT_OBJECT || type == STT_FILE)
  continue;
 if(symtab.symbols[i].st_value < address && address < symtab.symbols[i].st_value + symtab.symbols[i].st_size)
  {
  f.name = (char*) (symtab.strings + symtab.symbols[i].st_name);
  f.address = symtab.symbols[i].st_value;
  break;
  }
 }
return f;
}

void _backtrace(_symtab tab1, _symtab tab2)
{
unsigned int *fp, i, address;

asm volatile ("movl %%ebp, %0" : "=r" (fp));

printf("    Call trace:\n");

for (i = 0; i < 10; i++) 
 {
 fp = (unsigned int *)(*fp);
 if (!(*(fp + 1) && *fp))
  break;
 address = *(fp + 1);
 
 _function f;
 f.address = 0;
 if(tab1.length)
  f = get_function(tab1, address);
 if(tab2.length && !f.address)
  f = get_function(tab2, address);
  
 if(f.name != 0)
  printf("      %z%x%z: [%z<%X>%z] %z%s%z+0x%x\n", address < 0x10000000 ? LIGHTRED : LIGHTGREEN, i, LIGHTGRAY, LIGHTGREEN, address, LIGHTGRAY, LIGHTBLUE, f.name, LIGHTGRAY, address - f.address);
 else
  printf("      %z%x%z: [%z<%X>%z] (unknown)\n", address < 0x10000000 ? LIGHTRED : LIGHTGREEN, i, LIGHTGRAY, LIGHTRED, address, LIGHTGRAY);
 }
printf(" =====^^=%zKRN%z/%zUSR%z==^^=%zRES%z/%zUNK%z==^^=%zFUNC%z+OFFSET===================================\n", LIGHTRED, LIGHTGRAY, LIGHTGREEN, LIGHTGRAY,
	LIGHTGREEN, LIGHTGRAY, LIGHTRED, LIGHTGRAY, LIGHTBLUE, LIGHTGRAY);
}

void user_backtrace()
{
_backtrace(get_kernel_symtab(), get_user_symtab());
}

void raw_backtrace()
{
_backtrace(null, null);
}

void kernel_backtrace()
{
_backtrace(get_kernel_symtab(), null);
}