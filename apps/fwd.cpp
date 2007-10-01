#include <messages.h>
#include <system.h>
#include <string.h>

int main()
{
while(1)
 {
 Message msg;
 printf("fwd: waiting\n");
 if(receive(msg) == MSG_OK)
  printf("fwd: got msg t=%d\n", msg.type);
 else 
  printf("fwd: error\n");
 printf("fwd: forwarding\n", msg.type, msg.value1, msg.value2, msg.reply, msg.reply_length);
 msg.receiver = 4;
 if(forward(msg) == MSG_OK)
  printf("fwd: ok\n");
 else
  printf("fwd: error\n");
 }
}
