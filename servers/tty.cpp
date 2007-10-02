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

struct StdinThread
 {
 unsigned int task;
 unsigned int thread;
 };

List<StdinThread*>* threads;
Mutex threads_mutex;

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

void tprintf(char* fmt, ...)
{
va_list list;
va_start(list,fmt);
char buf[1000];
vsprintf(buf, fmt, list);
va_end(list);
SYSCALL1(2, buf);
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
    case 0x4B: //left arrow
    case 0x4D: //right arrow
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
      if(leftshift || rightshift)
       ascii = scancodes_shifted[scancode];
      else
       ascii = scancodes[scancode];
      }
     break;
     }
    }
   }
  break;
  }
 if(ascii)
  tprintf("%c", ascii);
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

tprintf("tty: new stdin thread %d: task %d\n", get_tid(), me->task);

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
  {
  char* k = "test1234";
  msg.reply = k;
  msg.reply_length = strlen(k) + 1;
  msg.type = frOk;
  }
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

start_thread(&keyboard_thread);
threads = NULL;
threads_mutex.unlock();

tprintf("tty: initialized\n");

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
     th->thread = start_thread(&stdin_thread);
     th->task = msg.sender;
     if(!threads)
      threads = new List<StdinThread*>(th);
     else
      threads->add_tail(new List<StdinThread*>(th));
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
   SYSCALL1(2, msg.data);
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