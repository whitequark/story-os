#include <messages.h>
#include <system.h>
#include <string.h>

int main()
{
while(1)
 {
 char data[100];
 Message msg;
 msg.data = data;
 msg.data_length = 50;
 printf("app2: waiting\n");
 if(receive(msg) == MSG_OK)
  printf("app2: got msg t=%d, v1=%d, v2=%d, d=%s, dl=%d, dr=%d\n", msg.type, msg.value1, msg.value2, msg.data, msg.data_length, msg.data_received);
 else 
  printf("app2: error\n");
 msg.type++;
 msg.value1 += 2;
 msg.value2 += 3;
 char* replys = "fuck yourself";
 msg.reply = replys;
 msg.reply_length = strlen(replys)+1;
 printf("app2: sending reply t=%d, v1=%d, v2=%d, r=%s, rl=%d\n", msg.type, msg.value1, msg.value2, msg.reply, msg.reply_length);
 if(reply(msg) == MSG_OK)
  printf("app2: sent\n");
 else
  printf("app2: error\n");
 }
}
