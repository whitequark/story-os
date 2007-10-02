#include <story.h>
#include <file.h>
#include <assert.h>
#include <string.h>

#define ZERO_ID 0
#define NULL_ID 1

int main()
{
File zero("/dev/zero"), null("/dev/null");
assert(zero.create() == frOk);
assert(null.create() == frOk);
assert(zero.mount(get_tid(), ZERO_ID) == frOk);
assert(null.mount(get_tid(), NULL_ID) == frOk);

while(1)
 {
 char data[4096];
 Message msg = {0};
 msg.data = data;
 msg.data_length = 4096;
 receive(msg);
 
 switch(msg.type)
  {
  case foResolve:
  if(!strcmp((char*) msg.data, ""))
   {
   msg.value1 = msg.value2;
   msg.value2 = 1;
   msg.type = frOk;
   }
  else
   msg.type = frFileNotFound;
  break;
  
  case foWrite:
  msg.type = frOk;
  break;
  
  case foRead:
  if(msg.value1 == ZERO_ID)
   {
   char reply[2048] = {0};
   msg.reply = reply;
   msg.reply_length = 2048 > msg.reply_length ? msg.reply_length : 2048;
   msg.type = frOk;
   }
  else if(msg.value1 == NULL_ID)
   {
   msg.reply = NULL;
   msg.reply_length = 0;
   msg.type = frOk;
   }
  break;
  
  default:
  msg.type = frCommandNotSupported;
  }
 reply(msg);
 }
}