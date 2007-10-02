#include <messages.h>
#include <system.h>
#include <string.h>

int main()
{
while(1)
 {
 char data[100];
 Message m = {0};
 m.data_length = 100;
 m.data = data;
 if(receive(m) != MSG_OK)
  {
  printf(" re ");
  while(1);
  }
 printf("R%d ", m.type);
 if(reply(m) != MSG_OK)
  {
  printf(" Re ");
  while(1);
  }
 }
}
