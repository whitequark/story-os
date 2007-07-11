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

#include <procman.h>
#include <mutex.h>
#include <filesystem.h>
#include <file.h>

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

bool keystate[6], escaped = 0;
const unsigned int LEFT_SHIFT = 0, RIGHT_SHIFT = 5, LEFT_ALT = 2, RIGHT_ALT = 3, LEFT_CTRL = 1, RIGHT_CTRL = 4;
Mutex* locker;
const unsigned int BUFFER_SIZE = 1000;
char* buffer;
unsigned int pointer;

void fs_thread()
{
Procman p;
Messenger m;
Filesystem fs;
fs.add("/input");
fs.add("/leds");
fs.show();
File keyboard("/dev/keyboard");
//keyboard.
keyboard.create();
while(1)
 {
 p.wait_for_message();
 
 Message msg, reply;
 reply.size = 0;
 reply.buffer = NULL;
 reply.type = rtOk;
 msg.size = 0; //after receive() size = real message size
 msg.buffer = NULL;
 m.receive(msg);
 
 bool ret;
 
 switch(msg.type)
  {
  default:
  reply.type = rtError;
  m.reply(reply);
  }
 }
}

void process_scancode(unsigned char scancode)
{
Procman p;
int ascii = 0;
switch(scancode) 
 {
 case 0x2A:        keystate[LEFT_SHIFT] = true;    break;
 case 0x2A + 0x80: keystate[LEFT_SHIFT] = false;   break;
 case 0x36:        keystate[RIGHT_SHIFT] = true;   break;
 case 0x36 + 0x80: keystate[RIGHT_SHIFT] = false;  break;
 case 0xE0: escaped = true; break;
  
 default:
 if(escaped)
  {
  switch(scancode)
   {
   case 0x1D:        keystate[RIGHT_CTRL] = true;  break;
   case 0x1D + 0x80: keystate[RIGHT_CTRL] = false; break;
   case 0x38:        keystate[RIGHT_ALT] = true;   break;
   case 0x38 + 0x80: keystate[RIGHT_ALT] = false;  break;
   }
  escaped = false;
  } 
 else 
  {
  switch(scancode)
   {
   case 0x1D:        keystate[LEFT_CTRL] = true;   break;
   case 0x1D + 0x80: keystate[LEFT_CTRL] = false;  break;
   case 0x38:        keystate[LEFT_ALT] = true;    break;
   case 0x38 + 0x80: keystate[LEFT_ALT] = false;   break;
     
   default:
   if(scancode < 0x80)
    if(keystate[LEFT_SHIFT] || keystate[RIGHT_SHIFT]) ascii = scancodes_shifted[scancode];
    else                                              ascii = scancodes[scancode];
   }
  }
 if(ascii != 0)
  {
  p.printf("%c", ascii);
  buffer[pointer++] = ascii;
  if(pointer == BUFFER_SIZE)
   pointer = 0;
  }
 }
}

int main()
{
Procman p;
Messenger m;

outb(0x60, 0xF4); //recalibrate
do { inb(0x60); } //empty buffer
while(inb(0x64) & 1);

locker = new Mutex;
buffer = new char[BUFFER_SIZE];
pointer = 0;

p.create_thread((void*) &fs_thread);

while(1)
 {
 while(!(inb(0x64) & 1));
 
 unsigned char scancode = inb(0x60);
 locker->lock();
 process_scancode(scancode);
 locker->unlock();
 }
}
