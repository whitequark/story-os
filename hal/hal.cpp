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

#include <colors.h>
#include <stdarg.h>
#include <hal.h>
#include <string.h>
#include <elf.h>

void HAL::panic(char *fmt, ...)
{
va_list vl;

va_start(vl, fmt);
textcolor(WHITE + BG_MAGENTA);
printf("Task %i has caused an unrecoverable error:\n", hal->taskman->current->index);
textcolor(7);
vprintf(fmt, vl);

for(;;);
}

void HAL::sti()
{
asm("sti");
}

void HAL::cli()
{
asm("cli");
}

unsigned int HAL::get_memory_amount()
{
return mbi->mem_upper;
}

void HAL::wait_for_debugger()
{
printf("Waiting for debugger... ");
static int dc;
while(!dc);
dc = 0;
printf("%zok%z\n", LIGHTGREEN, LIGHTGRAY);
}

char* HAL::option(char* opt_needed)
{
char* option = NULL;
char* cmd = (char*) mbi->cmdline;
 
while(1)
 {
 //search the space
 while(*cmd != ' ' && *cmd != 0)
  cmd++;
 if(!*cmd++)
  return NULL;
 //now cmd points to next char after space
 char* opt_end = cmd;
 //search next space or equal mark - end of option
 while(*opt_end != ' ' && *opt_end != '=' && *opt_end != 0)
  opt_end++;
 if(opt_end == cmd)
  continue; //it's not an option, simply 2 spaces
 else
  {
  option = (char*) kcalloc(1, opt_end - cmd + 1);
  strncpy(option, cmd, opt_end - cmd);
  if(!strcmp(option, opt_needed))
   {
   if(*opt_end == '=')
    {
    char* val_end = opt_end;
    while(*val_end != ' ' && *val_end != 0)
     val_end++;
    if(val_end - opt_end > 0)
     {
     kfree(option);
     option = (char*) kcalloc(1, val_end - opt_end + 1);
     strncpy(option, opt_end + 1, val_end - opt_end);
     }
    return option;
    }
   return option;
   }
  else
   kfree(option);
  }
 }
return NULL;
}

HAL::HAL(multiboot_info_t* _mbi): lfb((char*)0xB8000), mbi(_mbi)
{
}

KernelTerminal::KernelTerminal() : lfb((short unsigned*)hal->lfb), cursorx(0), cursory(0)
{
color = LIGHTGRAY;
}

#define VGA_CRT_IC      0x3D4
#define VGA_CRT_DC      0x3D5

void KernelTerminal::put_char(char ch)
{
if(ch == 0)
 return;
if(ch != '\n')
 lfb[(cursorx++) + cursory * 80] = color << 8 | ch;
else
 {
 cursorx = 0;
 cursory++;
 }
if(cursorx == 80)
 {
 cursorx = 0;
 cursory++;
 }
if(cursory == 25)
 {
 cursory = 24;
 cursorx = 0;
 memcpy(lfb, lfb + 80, 24*80*2);
 for(int i = 80*24; i < 80*25; i++)
  lfb[i] = 0x0700;
 }
unsigned short offset = cursorx + cursory * 80;
hal->outb(VGA_CRT_IC, 0x0f);
hal->outb(VGA_CRT_DC, offset & 0xff);
offset >>= 8;
hal->outb(VGA_CRT_IC, 0x0e);
hal->outb(VGA_CRT_DC, offset & 0xff);
}

void KernelTerminal::clear()
{
int i;
for(i = 0; i < 80*25; i++)
 lfb[i] = 0x0700;
}

void KernelTerminal::set_color(unsigned char color)
{
this->color = color;
}

void KernelTerminal::put_char_raw(unsigned char ch, unsigned char color)
{
lfb[(cursorx++) + cursory * 80] = color << 8 | ch;
if(cursorx == 80)
 {
 cursorx = 0;
 cursory++;
 }
if(cursory == 25)
 {
 cursory = 24;
 cursorx = 0;
 memcpy(lfb, lfb + 80, 24*80*2);
 for(int i = 80*24; i < 80*25; i++)
  lfb[i] = 0x0700;
 }
unsigned short offset = cursorx + cursory * 80;
hal->outb(VGA_CRT_IC, 0x0f);
hal->outb(VGA_CRT_DC, offset & 0xff);
offset >>= 8;
hal->outb(VGA_CRT_IC, 0x0e);
hal->outb(VGA_CRT_DC, offset & 0xff);
}

void putchar(char ch)
{
hal->terminal->put_char(ch);
}

void textcolor(char color)
{
hal->terminal->set_color(color);
}