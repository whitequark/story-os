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
#include <stdio.h>
#include <system.h>
#include <assert.h>
#include <ipc.h>
#include <terminal.h>

void putchar(char c);

const char scancodes_shifted[] = {
  0,
  0, //ESC
  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
  '_', '+', 
  8, //BACKSPACE
  '\t',//TAB
  'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
  '\n', //ENTER
  0, //CTRL
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
  0, //LEFT SHIFT,
  '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
  0, //RIGHT SHIFT,
  '*', //NUMPAD
  0, //ALT
  ' ', //SPACE
  0, //CAPSLOCK
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //F1 - F10
  0, //NUMLOCK
  0, //SCROLLOCK
  0, //HOME
  0, 
  0, //PAGE UP
  '-', //NUMPAD
  0, 0,
  0, //(r)
  '+', //NUMPAD
  0, //END
  0, 
  0, //PAGE DOWN
  0, //INS
  0, //DEL
  0, //SYS RQ
  0, 
  0, 0, //F11-F12
  0,
  0, 0, 0, //F13-F15
  0, 0, 0, 0, 0, 0, 0, 0, 0, //F16-F24
  0, 0, 0, 0, 0, 0, 0, 0
};

const char scancodes[] = {
  0,
  0, //ESC
  '1','2', '3', '4', '5', '6', '7', '8', '9', '0',
  '-', '=', 
  8, //BACKSPACE
  '\t',//TAB
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
  '\n', //ENTER
  0, //CTRL
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
  0, //LEFT SHIFT,
  '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
  0, //RIGHT SHIFT,
  '*', //NUMPAD
  0, //ALT
  ' ', //SPACE
  0, //CAPSLOCK
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //F1 - F10
  0, //NUMLOCK
  0, //SCROLLOCK
  0, //HOME
  0, 
  0, //PAGE UP
  '-', //NUMPAD
  0, 0,
  0, //(r)
  '+', //NUMPAD
  0, //END
  0, 
  0, //PAGE DOWN
  0, //INS
  0, //DEL
  0, //SYS RQ
  0, 
  0, 0, //F11-F12
  0,
  0, 0, 0, //F13-F15
  0, 0, 0, 0, 0, 0, 0, 0, 0, //F16-F24
  0, 0, 0, 0, 0, 0, 0, 0
};

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

int main()
{
printf("Initializing keyboard... ");
outb(0x21, 0);
outb(0x60, 0xF4);
while(inb(0x64) & 1)
 inb(0x60);
assert(Interface("terminal").add() == 0);
printf("%zok%z\n", LIGHTGREEN, LIGHTGRAY);

MessageQuery q;

bool leftctrl = 0, rightctrl = 0, leftshift = 0, rightshift = 0, leftalt = 0, rightalt = 0, escaped = 0;

for(;;)
 {
 unsigned char scancode;
 while(q.pending())
  {
  switch(q.type())
   {
   case Terminal::mtPutChar:
   if(q.length() == 1)
    {
    char c;
    q.data(&c);
    putchar(c);
    }
   Message(NULL, 0).reply();
   break;
   
   case Terminal::mtPutString:
   if(q.length() > 1)
    {
    char c[q.length()];
    q.data(c);
    for(int i = 0; i < q.length(); i++)
     putchar(c[i]);
    }
   else
    {
    char c;
    q.data(&c);
    putchar(c);
    }    
   Message(NULL, 0).reply();
   break;
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
   continue;
   
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
 if(leftshift && rightshift)
  *((int*)0) = 0;
 /*char* s = "csa_asc";
 s[0] = leftctrl ? 'C' : 'c';
 s[1] = leftshift ? 'S' : 's';
 s[2] = leftalt ? 'A' : 'a';
 s[4] = rightalt ? 'A' : 'a';
 s[5] = rightshift ? 'S' : 's';
 s[6] = rightctrl ? 'C' : 'c';
 int n;
 for(n = 0; n < 7; n++)
  {
  unsigned short* bf = (unsigned short*) (0xc0000000 + n*2 + 38*2);
  *bf = s[n] | (WHITE << 8);
  }*/
 }
}