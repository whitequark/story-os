#include <story.h>
#include <string.h>

int main()
{
while(1)
 {
 printf("slsh# ");
 
 char buf[1000], ch;
 int pointer = 0;
 while((ch = getch()) != '\n')
  {
  if(ch == 8)
   if(pointer > 0)
    {
    pointer--;
    printf("\x8");
    }
  if(ch != 8)
   {
   buf[pointer++] = ch;
   printf("%c", ch);
   }
  }
 buf[pointer] = 0;
 printf("\n");
 
 if(!strcmp(buf, ""))
  continue;
 
 char *cmd = buf, *param = NULL;
 for(int i = 0; i < strlen(buf); i++)
  if(buf[i] == ' ')
   {
   param = &buf[i+1];
   buf[i] = 0;
   }
 
 if(!strcmp(cmd, "help"))
  printf("Commands:\n help - display this help\n version - display shell and OS version\n");
 else if(!strcmp(cmd, "version"))
  printf("Story Light SHell version 1.0\nStory OS version %s\n", VERSION);
 else
  printf("Unknown command\n");
 }
}