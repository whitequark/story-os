#include <story.h>
#include <file.h>
#include <assert.h>
#include <string.h>

#define ROOT_ID 1

int main()
{
File tty("/dev/tty");
assert(tty.create() == frOk);
assert(tty.mount(get_tid()) == frOk);
SYSCALL1(2, "tty: initialized\n");

while(1)
 {
 char data[1024];
 Message msg = {0};
 msg.data = data;
 msg.data_length = 1024;
 receive(msg);
 
 switch(msg.type)
  {
  case foResolve:
  if(!strcmp((char*) msg.data, "")) // root
   {
   msg.value1 = ROOT_ID;
   msg.value2 = 1;
   msg.type = frOk;
   }
  else
   msg.type = frFileNotFound;
  break;
  
  case foWrite:
  if(msg.value1 == 1)
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