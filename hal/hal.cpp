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

#include <colors.h>
#include <stdarg.h>
#include <stdio.h>
#include <hal.h>
#include <string.h>

void HAL::panic(char *fmt, ...)
{
va_list vl;

va_start(vl, fmt);
textcolor(WHITE + BG_MAGENTA);
printf("Task %i has caused an unrecoverable error:\n", hal->taskman->current->index); //TODO current task
textcolor(7);
vprintf(fmt, vl);

for(;;);
}

void HAL::outb(unsigned short port, unsigned char value)
{
asm("outb %b0,%w1":: "a"(value), "d"(port));
}

unsigned char HAL::inb(unsigned short port)
{
char value;
asm("inb %w1, %b0": "=a"(value): "d"(port));
return value;
}

void HAL::outw(unsigned short port, unsigned short value)
{
asm("outw %%ax, %%dx"::"a"(value),"d"(port));
}

unsigned short HAL::inw(unsigned short port)
{
short value;
asm("inw %%dx, %%ax":"=a"(value):"d"(port));
return value;
}

void HAL::sti_c()
{
int_block_count--;
if(int_block_count < 0)
 {
 int_block_count = 0;
 sti();
 } 
}

void HAL::cli_c()
{
int_block_count++;
cli();
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
return mbi.mem_upper;
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
char* cmd = (char*) mbi.cmdline;
 
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
  option = (char*) calloc(1, opt_end - cmd + 1);
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
     free(option);
     option = (char*) calloc(1, val_end - opt_end + 1);
     strncpy(option, opt_end + 1, val_end - opt_end);
     }
    return option;
    }
   return option;
   }
  else
   free(option);
  }
 }
return NULL;
}

HAL::HAL(multiboot_info_t* _mbi): lfb((char*)0xB8000), mbi(*_mbi)
{
}