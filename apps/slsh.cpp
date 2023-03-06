#include <story.h>
#include <string.h>

int main()
{
while(1)
 {
 printf("slsh# ");
 
 char buf[100] = {0}, ch;
 int pointer = 0;
 while((ch = getch()) != '\n')
  {
  switch(ch)
   {
   case 8:
   if(pointer > 0)
    {
    pointer--;
    printf("\x8");
    }
   break;
   
   case 0x1b:
   case 0x1d:
   case 0x1c:
   case 0x1a:
   break;
   
   default:
   memcpy(&buf[pointer + 1], &buf[pointer], pointer);
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
  printf("Commands:\n"
         " help - display this help\n"
         " version - display shell and OS version\n"
         " quit - exit the shell\n"
        );
 else if(!strcmp(cmd, "version"))
  printf("Story Light SHell version 1.0\nStory OS version %s\n", VERSION);
 else if(!strcmp(cmd, "quit"))
  die(0);
 else
  {
  int tid = exec(cmd, param);
  if(tid)
   wait_die(tid);
  else
   printf("Cannot launch `%s'\n", cmd);
  printf("\n");
  }
 }
}
