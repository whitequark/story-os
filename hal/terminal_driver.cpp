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

#include <terminal_driver.h>
#include <hal.h>
#include <string.h>

//FIXME: remove it

KernelTerminalDriver::KernelTerminalDriver() : lfb((short unsigned*)hal->lfb), cursorx(0), cursory(0)
{
update_cursor();
color = LIGHTGRAY;}

void KernelTerminalDriver::put_char(char ch, int x, int y)
{
if(ch == 0)
 return;
if(x == -1 && y == -1)
 {
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
  memset(lfb + 80*24, 0, 80*2);
  }
 }
else if(x >= 0 && x <= 80 && y >= 0 && y <= 25)
 lfb[80*y + x] = color << 8 | ch;
update_cursor();
}

void KernelTerminalDriver::update_cursor()
{
unsigned int cursor = cursorx + cursory * 80;
hal->outb(0x3D4, 14);
hal->outb(0x3D5, cursor >> 8);
hal->outb(0x3D4, 15);
hal->outb(0x3D5, cursor);
}

void KernelTerminalDriver::clear()
{
int i;
for(i = 0; i < 80*25; i++)
 lfb[i] = 0;
}

void KernelTerminalDriver::set_color(unsigned char color)
{
this->color = color;
}