#include <messages.h>
#include <system.h>
#include <string.h>

int main()
{
Message m;
char* data = "fuck you";
char reply[100];
m.type = 135;
m.value1 = 10;
m.value2 = 20;
m.data = data;
m.data_length = strlen(data) + 1;
m.reply = reply;
m.reply_length = 20;
m.receiver = 3;
printf("app1: sending t=%d, v1=%d, v2=%d, d=%s, dl=%d\n", m.type, m.value1, m.value2, m.data, m.data_length);
int k = send(m);
if(k == MSG_OK)
 printf("app1: got reply: t=%d, v1=%d, v2=%d, dr=%d, r=%s, rl=%d, rs=%d\n", m.type, m.value1, m.value2, m.data_received, m.reply, m.reply_length, m.reply_sent);
else
 printf("app1: error %d\n", k);
}
