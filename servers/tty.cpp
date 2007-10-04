#include <story.h>
#include <file.h>
#include <assert.h>
#include <string.h>
#include <list.h>
#include <service.h>
#include <vsprintf.h>
#include <mutex.h>

#define ROOT_ID 1
#define STDIN 1
#define STDOUT 2

#define CONSOLES 8

#define KEYBOARD_BUFFER_SIZE 100

struct StdinThread
 {
 unsigned int task;
 unsigned int thread;
 unsigned int console;
 };

List<StdinThread*>* threads;
Mutex threads_mutex;

struct VirtualConsole
 {
 unsigned short* data;
 unsigned short offset;
 unsigned char color;
 char* kbd_buffer;
 unsigned int kbd_pointer;
 Mutex kbd_mutex;
 Mutex stdin_read;
 unsigned int current_thread;
 bool locked;
 };

VirtualConsole virtual_console[CONSOLES];
unsigned int current_console;
unsigned short* lfb;
Mutex console_mutex;

char scancodes[] = {
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

char scancodes_shifted[] = {
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

void update_cursor(unsigned short offset)
{
outb(0x3d4, 0x0f);
outb(0x3d5, offset & 0xff);
offset >>= 8;
outb(0x3d4, 0x0e);
outb(0x3d5, offset & 0xff);
}

void putchar(char ch)
{
if(ch == 0)
 return;

console_mutex.lock();
VirtualConsole &console = virtual_console[current_console];

switch(ch)
 {
 case '\n':
 console.offset += 80;
 console.offset -= console.offset % 80;
 break;
 
 case 8:
 lfb[--console.offset] = 0x0700;
 break;
 
 case 0x1b:
 console.offset--;
 break;
 
 case 0x1d:
 console.offset++;
 break;
 
 default:
 lfb[console.offset++] = console.color << 8 | ch;
 break;
 }

if(console.offset == 25*80)
 {
 console.offset -= 80;
 memcpy(lfb, lfb + 80, 24*80*2);
 for(int i = 80*24; i < 80*25; i++)
  lfb[i] = 0x0700;
 }
update_cursor(console.offset);
console_mutex.unlock();
}

void tprintf(char* fmt, ...)
{
va_list list;
va_start(list,fmt);
char buf[1000];
vsprintf(buf, fmt, list);
va_end(list);
for(int i = 0; i < strlen(buf); i++)
 putchar(buf[i]);
}

void switch_console(unsigned int number)
{
if(current_console == number);
console_mutex.lock();
memcpy(virtual_console[current_console].data, lfb, 25*80*2);
memcpy(lfb, virtual_console[number].data, 25*80*2);
update_cursor(virtual_console[number].offset);
current_console = number;
console_mutex.unlock();
if(virtual_console[number].offset == 0)
 tprintf("Virtual Console #%d\n", number + 1);
}

int keyboard_thread()
{
gain_io_privilegies();
attach_irq(1);
while(inb(0x64) & 1)
 inb(0x60);
outb(0x61, 0xFF);

bool leftshift, rightshift, leftctrl, rightctrl, leftalt, rightalt, escaped;

while(1)
 {
 while(!(inb(0x64) & 1));
 unsigned char scancode = inb(0x60);
 unsigned char ascii = 0;

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

  default:
  if(escaped) 
   {
   switch(scancode) 
    {
    case 0x48: //up arrow
    ascii = 0x1a;
    break;
    
    case 0x4B: //left arrow
    ascii = 0x1b;
    break;
    
    case 0x4D: //right arrow
    ascii = 0x1d;
    break;
    
    case 0x50: //down arrow
    ascii = 0x1c;
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
   if(scancode == 0xE0) 
    escaped = true;
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
      if(leftalt || rightalt)
       {
       if(scancode >= 0x3b && scancode < 0x3b + CONSOLES)
        switch_console(scancode - 0x3b);
       }
      else
       {
       if(leftshift || rightshift)
        ascii = scancodes_shifted[scancode];
       else
        ascii = scancodes[scancode];
       }
      }
     break;
     }
    }
   }
  break;
  }
 if(ascii)
  {
  VirtualConsole& console = virtual_console[current_console];
  if(console.locked)
   continue;
  console.kbd_mutex.lock();
  if(console.kbd_pointer >= KEYBOARD_BUFFER_SIZE - 1)
   continue;
  console.kbd_buffer[console.kbd_pointer++] = ascii;
  console.kbd_mutex.unlock();
  }
 }
}

int stdin_thread()
{
threads_mutex.lock();
List<StdinThread*>* i;
StdinThread* me = NULL;
iterate_list(i, threads)
 if(i->item->thread == get_tid())
  {
  me = i->item;
  break;
  }
threads_mutex.unlock();
if(!me)
 return 0;

VirtualConsole& console = virtual_console[me->console];

while(1)
 {
 char data[MAX_PATH];
 Message msg = {0};
 msg.data = data;
 msg.data_length = MAX_PATH;
 receive(msg);
 
 unsigned int pointer = 0;
 char ch;
 
 switch(msg.type)
  {
  case foResolve:
  if(!strcmp((char*) msg.data, "") && msg.value2 == STDIN)
    {
    msg.value1 = STDIN;
    msg.value2 = 1;
    msg.type = frOk;
    }
  else
   msg.type = frFileNotFound;
  break;
  
  case foRead:
  if(msg.value1 == STDIN)
   {
   console.stdin_read.lock();
   console.current_thread = get_tid();
   msg.reply = new char[msg.reply_length];
   if(msg.reply_length > 1)
    {
    while(pointer < msg.reply_length)
     {
     while(console.kbd_pointer == 0);
     console.kbd_mutex.lock();
     for(int i = 0; i < console.kbd_pointer; i++)
      {
      if(console.kbd_buffer[i] >= 0x20)
       {
       ((char*)msg.reply)[pointer++] = console.kbd_buffer[i];
       putchar(console.kbd_buffer[i]);
       }
      if(console.kbd_buffer[i] == 8)
       pointer--;
       
      if(pointer == -1)
       pointer = 0;
      else if(console.kbd_buffer[i] == 8)
       putchar(8);
      }
     console.kbd_pointer = 0;
     console.kbd_mutex.unlock();
     }
    }
   else
    {
    while(console.kbd_pointer == 0);
    console.kbd_mutex.lock();
    ch = console.kbd_buffer[0];
    memcpy(&console.kbd_buffer[0], &console.kbd_buffer[1], console.kbd_pointer);
    console.kbd_pointer--;
    console.kbd_mutex.unlock();
    msg.reply = &ch;
    }
   console.stdin_read.unlock();
   msg.type = frOk;
   }
  else
   msg.type = frFileNotFound;
  break;
  
  default:
  msg.type = frCommandNotSupported;
  }
 reply(msg);
 }
}

int main()
{
File stdout("/dev/stdout");
assert(stdout.create() == frOk);
assert(stdout.mount(get_tid(), STDOUT) == frOk);
File stdin("/dev/stdin");
assert(stdin.create() == frOk);
assert(stdin.mount(get_tid(), STDIN) == frOk);

current_console = 0;
for(int i = 0; i < CONSOLES; i++)
 {
 virtual_console[i].data = new unsigned short[25*80];
 for(int j = 0; j < 25*80; j++)
  virtual_console[i].data[j] = 0x0700;
 virtual_console[i].offset = 0;
 virtual_console[i].color = 0x07;
 virtual_console[i].kbd_buffer = new char[KEYBOARD_BUFFER_SIZE];
 virtual_console[i].kbd_pointer = 0;
 virtual_console[i].kbd_mutex.unlock();
 virtual_console[i].stdin_read.unlock();
 virtual_console[i].locked = false;
 }
lfb = (unsigned short*) attach_memory(1, 0xb8000);

memcpy(virtual_console[CONSOLES-1].data, lfb, 25*80*2);
virtual_console[CONSOLES-1].locked = true;
virtual_console[CONSOLES-1].offset = 25*80;

for(int j = 0; j < 25*80; j++)
 ((unsigned short*)lfb)[j] = 0x0700;

console_mutex.unlock();
switch_console(0);

start_thread(&keyboard_thread);
threads = NULL;
threads_mutex.unlock();

tprintf("tty: initialized\n");

start_thread(&stdin_thread);
start_thread(&stdin_thread);

while(1);

while(1)
 {
 char data[MAX_PATH];
 Message msg = {0};
 msg.data = data;
 msg.data_length = MAX_PATH;
 receive(msg);
 
 switch(msg.type)
  {
  case foResolve:
  if(!strcmp((char*) msg.data, "")) // root
   {
   if(msg.value2 == STDOUT)
    {
    msg.value1 = STDOUT;
    msg.value2 = 1;
    msg.type = frOk;
    }
   else if(msg.value2 == STDIN)
    {
    StdinThread* th = NULL;
    List<StdinThread*>* i;
    iterate_list(i, threads)
     if(i->item->task == msg.sender)
      {
      th = i->item;
      break;
      }
    if(!th)
     {
     threads_mutex.lock();
     th = new StdinThread;
     if(!threads)
      threads = new List<StdinThread*>(th);
     else
      threads->add_tail(new List<StdinThread*>(th));
     th->task = msg.sender;
     th->console = 0;
     th->thread = start_thread(&stdin_thread);
     threads_mutex.unlock();
     }
    msg.receiver = th->thread;
    forward(msg);
    continue;
    }
   else
    msg.type = frFileNotFound;
   }
  else
   msg.type = frFileNotFound;
  break;
  
  case foWrite:
  if(msg.value1 == STDOUT)
   {
   tprintf("%s", msg.data);
   msg.type = frOk;
   }
  else
   msg.type = frFileNotFound;
  break;
  
  default:
  msg.type = frCommandNotSupported;
  }
 reply(msg);
 }
}