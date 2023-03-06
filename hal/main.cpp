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

#include <multiboot.h>
#include <story.h>
#include <stdlib.h>
#include <assert.h>
#include <hal.h>
#include <colors.h>
#include <core.h>
#include <string.h>

/*
				MEMORY MAP
0000 0000-0009 FFFF			reserved area: BIOS data and other stuff	898K
	0000 0000-0000 03FF		Real-Mode interrupt table			4*0x100
	0000 0400-0000 04FF		BIOS data area					0x100 
	0000 0500-0009 FBFF		Conventional memory (UNUSED)			0x9F700
	0009 FC00-0009 FFFF		Extended BIOS data area				0x400

000A 0000-000F FFFF			ROM area
	000A 0000-000B FFFF		VGA memory					128K
		000A 0000-000A FFFF	VGA framebuffer					64K
		000B 0000-000B 7FFF	Text monochrome					32K
		000B 8000-000B FFFF	Text color					32K
	000C 0000-000C 7FFF		VGA BIOS					32K
	000F 0000-000F FFFF		Motherboard BIOS				64K

				All my stuff: begins at 1M
0010 0000-FEBF FFFF			Extended memory
	0010 0000-001F FFFF		GRUB
	0020 0000-003F FFFF		Kernel & modules				4M
	0040 2000-XXXX XXXX		Free memory

FEC0 0000-FFFF FFFF			Internal stuff
*/

/*
				GDT
0		NULL
1		System Code
2		System Data
3		Application Code
4		Application Data
5		Application TSS
6		Scheduler TSS
*/

HAL* hal;

#ifdef _ENABLE_GDB_STUB_
extern "C" void set_debug_traps();
extern "C" void breakpoint();
#endif

extern "C" void entry(unsigned long magic, multiboot_info_t* multiboot_info)
{
unsigned int mm_position = 0x00400000;
if(multiboot_info->mods_count != 0)
 {
 module_t* mod;
 for(mod = (module_t*)multiboot_info->mods_addr; mod->mod_start != NULL; mod++)
  mm_position = mod->mod_end + 0x1000;
 }

MemoryManager* mm = new (mm_position) MemoryManager(mm_position + sizeof(*mm) + 0x1000, multiboot_info->mem_upper * 1024 + 0x100000);

kinit_malloc();

hal = new HAL(multiboot_info);
hal->mm = mm;

hal->terminal = new KernelTerminal;
hal->terminal->clear();

if(magic != MULTIBOOT_BOOTLOADER_MAGIC)
 hal->panic("Invalid magic number! I can be booted only with multiboot bootloader, e.g. GRUB!");

printf("%zStory OS%z version %z%s (build %i)%z, (C) 2007 Catherine 'whitequark'\n", LIGHTBLUE, WHITE, LIGHTGREEN, VERSION, BUILD, WHITE);
printf("Compiled %s, %s\n", __DATE__, __TIME__);
textcolor(LIGHTGRAY);
printf("Thanks to Legos, DinamytE, SadKo and all OSDev.ru community.\n\n");

/*for(int i = 0; i < 8; i++)
 {
 for(int j = 0; j < 16; j++)
  hal->terminal->put_char_raw(i * 32 + j, GREEN);
 printf(" | ");
 for(int j = 16; j < 32; j++)
  hal->terminal->put_char_raw(i * 32 + j, GREEN);
 printf("\n");
 }

for(int i = 0; i < 8; i++)
 {
 for(int j = 0; j < 16; j++)
  hal->terminal->put_char_raw(i * 32 + j, GREEN | 8);
 printf(" | ");
 for(int j = 16; j < 32; j++)
  hal->terminal->put_char_raw(i * 32 + j, GREEN | 8);
 printf("\n");
 }

hal->outw(0x3c4, 0x0100); //sync reset
hal->outw(0x3c4, 0x0402); //write plane 2
hal->outw(0x3c4, 0x0704); //sequential addressing
hal->outw(0x3c4, 0x0300); //clear reset

hal->outw(0x3ce, 0x0204); //read plane 2
hal->outw(0x3ce, 0x0005); //disable odd-even addressing
hal->outw(0x3ce, 0x0006); //start map at 0xA0000

int symbol_height = 16, symbol, table = 2;

memcpy((void*) (0xa0000 + symbol_height * 256 * 2 * table), (void*) 0xa0000, 256 * 2 * symbol_height);

for(int i = 0xa0000 + 256 * 2 * table * symbol_height + 2 * symbol_height; i < 0xa0000 + 256 * 2 * table * symbol_height + 2 * 256 * symbol_height; i++)
  *((char*) i) = *((char*) ((i - 256 * 2 * table * symbol_height) / symbol_height * symbol_height) + 15 - (i - 256 * 2 * table * symbol_height) % symbol_height);

hal->outw(0x3c4, 0x0100); //sync reset
hal->outw(0x3c4, 0x0302); //write planes 0&1
hal->outw(0x3c4, 0x0304); //odd-even addressing
hal->outw(0x3c4, 0x0403); //symbol tables D3=0 -> 0, D3=1 -> 1
hal->outw(0x3c4, 0x0300); //clear reset

hal->outw(0x3ce, 0x0004); //read map 0
hal->outw(0x3ce, 0x1005); //enable odd-even addressing
hal->outw(0x3ce, 0x0E06); //start map at 0xB8000*/

printf("%zInitializing HAL...%z ", GREEN, LIGHTGRAY);

hal->gdt = new GDT;
hal->gdt->add_descriptor(new NullDescriptor());

int num;

//0x00000000=>0xFFFFFFFF readable 32 bit code segment with dpl=0
num = hal->gdt->add_descriptor(new SegmentDescriptor(0, 0xFFFFFFFF, true, true, 0, true));
hal->sys_code = hal->gdt->make_segment(num, 0);

//0x00000000=>0xFFFFFFFF writable 32 bit data segment with dpl=0
num = hal->gdt->add_descriptor(new SegmentDescriptor(0, 0xFFFFFFFF, false, true, 0, true));
hal->sys_data = hal->gdt->make_segment(num, 0);

//0x00000000=>0xFFFFFFFF readable 32 bit code segment with dpl=3
num = hal->gdt->add_descriptor(new SegmentDescriptor(0, 0xFFFFFFFF, true, true, 3, true));
hal->app_code = hal->gdt->make_segment(num, 3);

//0x00000000=>0xFFFFFFFF writable 32 bit data segment with dpl=3
num = hal->gdt->add_descriptor(new SegmentDescriptor(0, 0xFFFFFFFF, false, true, 3, true));
hal->app_data = hal->gdt->make_segment(num, 3);

hal->gdt->install();

hal->idt = new IDT;
hal->idt->install();
hal->idt->register_exceptions();
hal->idt->register_irqs();

hal->pic = new PIC;
hal->pic->remap(0x20, 0x28);

hal->syscalls = new SyscallManager();

hal->paging = new Paging();
hal->paging->enable();

hal->clock = new Clock();

hal->taskman = new TaskManager();

printf("%zCOMPLETE%z\n", GREEN, LIGHTGRAY);

core = new Core(multiboot_info);

printf("%zEnabling taskswitching%z\n\n", LIGHTRED, LIGHTGRAY);

hal->sti();
hal->pic->unmask(0);
hal->taskman->mt(true);
hal->taskman->start();

for(;;);
}

#ifdef _ENABLE_KERNEL_SERIAL_

#define PORT 0x3f8   /* COM1 */

void init_serial() 
{
hal->outb(PORT + 1, 0x00);    // Disable all interrupts
hal->outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
hal->outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
hal->outb(PORT + 1, 0x00);    //                  (hi byte)
hal->outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
hal->outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
hal->outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

int c = 0;
void serial_put(char ch)
{
if(c==0) 
 {
 init_serial(); 
 c = 1; 
 }
while (hal->inb(PORT + 5) & 0x20 == 0);
int i; for(i=0;i<100;i++);
//textcolor(LIGHTRED); putchar(ch);
hal->outb(PORT, ch);
}

char serial_get() 
{
while ((hal->inb(PORT + 5) & 1) == 0);
char ch = hal->inb(PORT);
//textcolor(YELLOW); putchar(ch);
return ch;
}

#endif

#ifdef _ENABLE_SERIAL_GDB_STUB_

#ifndef _ENABLE_KERNEL_SERIAL_
#error Serial GDB stub needs '_ENABLE_KERNEL_SERIAL_'!
#endif

extern "C" int getDebugChar()
{
return serial_get();
}

extern "C" void putDebugChar(int ch)
{
serial_put(ch);
}

extern "C" void exceptionHandler (int exception_number, void *exception_address)
{
hal->idt->set_isr(exception_number, exception_address);
}

extern "C" void printf(char* format, ...)
{
va_list args;
textcolor(LIGHTGRAY);
va_start(args, format);
vprintf(format, args);
va_end(args);
}

#endif