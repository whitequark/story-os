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
#include <system.h>
#include <assert.h>
#include <ipc.h>
#include <terminal.h>
#include <string.h>
#include <colors.h>

#define VGA_CRT_IC      0x3D4
#define VGA_CRT_DC      0x3D5

void outb(unsigned short port, unsigned char value)
{
asm("outb %b0,%w1":: "a"(value), "d"(port));
}

unsigned char inb(unsigned short port)
{
char value;
asm("inb %w1, %b0": "=a"(value): "d"(port));
return value;
}

int cursorx, cursory;
char color;

void update_cursor()
{
unsigned short offset = cursorx + cursory * 80;
outb(VGA_CRT_IC, 0x0f);
outb(VGA_CRT_DC, offset & 0xff);
offset >>= 8;
outb(VGA_CRT_IC, 0x0e);
outb(VGA_CRT_DC, offset & 0xff);
}

void putchar(char ch)
{
unsigned short* lfb = (unsigned short*) 0xf0000000;
if(ch == 0)
 return;

if(ch == 8)
 cursorx--;
else if(ch == '\n')
 {
 cursorx = 0;
 cursory++;
 }
else
 lfb[(cursorx++) + cursory * 80] = color << 8 | ch;
 
if(cursorx == 80)
 { cursorx = 0; cursory++; }
if(cursorx == -1)
 { cursorx = 79; cursory--; }
if(cursory == 25)
 {
 cursory = 24;
 cursorx = 0;
 memcpy(lfb, lfb + 80, 24*80*2);
 memset(lfb + 80*24, 0, 80*2);
 }
if(cursory == -1)
 { cursory = 0; cursorx = 0; }
 
if(ch == 8)
 lfb[cursorx + cursory * 80] = 0x0700;
 
update_cursor();
}

void puts(char* s)
{
for(int i = 0; i < strlen(s); i++)
 putchar(s[i]);
}

int main()
{
cursorx = 0;
cursory = 0;
color = LIGHTGRAY;
map_pages(0x000B8000, 0xf0000000, 1);

unsigned short* lfb = (unsigned short*) 0xf0000000;
for(int k = 0; k < 25 * 80; k++)
 lfb[k] = 0x0700;
update_cursor();

puts("Story OS comes with ABSOLUTELY NO WARRANTY; for details type `cat warranty'\n");
puts("This is free software, and you are welcome to redistribute it\n");
puts("under certain conditions; type `cat copying' for details.\n");
puts("(I think, some kind of shell will write it. Sometimes:)\n\n");

outb(0x21, 0);
outb(0x60, 0xF4);
while(inb(0x64) & 1)
 inb(0x60);
Interface("terminal").add();

MessageQuery q;

bool leftctrl = 0, rightctrl = 0, leftshift = 0, rightshift = 0, leftalt = 0, rightalt = 0, escaped = 0;

for(;;)
 {
 unsigned char scancode;
 while(q.pending())
  {
  unsigned int t = q.type();
  if(t == Terminal::mtPutString)
   {
   char s[q.length()];
   q.data(s);
   for(int i = 0; i < q.length(); i++)
    putchar(s[i]);
   Message(NULL, 0).reply();
   }
  else if(t == Terminal::mtColor)
   {
   if(q.length() == 1)
    q.data(&color);
   Message(NULL, 0).reply();
   }
  }
  
 if(inb(0x64) & 1)
  {
  scancode = inb(0x60);

  int ascii = 0;
  switch(scancode) 
   {
   case 0x2A:
   leftshift = true;
   break;
 
   case 0x36: 
   rightshift = true;
   break;
 
   case 0x2A + 0x80:
   leftshift = false;
   break;
   
   case 0x36 + 0x80:
   rightshift = false;
   break;
   
   case 0xE0:
   escaped = true;
   break;
   
   default:
   if(escaped)
    {
    switch(scancode)
     {
     case 0x48: //up arrow
     break;

     case 0x4B: //left arrow
     break;

     case 0x4D: //right arrow
     break;

     case 0x50: //down arrow
     break;
     
     case 0x1D:
     rightctrl = true;
     break;

     case 0x1D + 0x80:
     rightctrl = false;
     break;
     
     case 0x38:
     rightalt = true;
     break;

     case 0x38 + 0x80:
     rightalt = false;
     break;
     }
    escaped = false;
    } 
   else 
    {
    switch(scancode)
     {
     case 0x1D:
     leftctrl = true;
     break;

     case 0x1D + 0x80:
     leftctrl = false;
     break;
     
     case 0x38:
     leftalt = true;
     break;

     case 0x38 + 0x80:
     leftalt = false;
     break;
     
     default:
     if(scancode < 0x80)
      {
      if(leftshift || rightshift)
       ascii = scancodes_shifted[scancode];
      else
       ascii = scancodes[scancode];
      }
     }
    }
   if(ascii != 0)
    putchar(ascii);
   }
  }
 }
}