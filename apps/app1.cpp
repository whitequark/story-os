#include <messages.h>
#include <system.h>
#include <string.h>

int main()
{
int i = 0;
while(1)
 {
 //delay(10);
 char* s = "test";
 printf("S%d ", ++i);
 Message m = {0};
 m.data = s;
 m.data_length = 5;
 m.type = i;
 m.receiver = 4;
 if(send(m) != MSG_OK)
  {
  printf(" se ");
  while(1);
  }
 }
}
