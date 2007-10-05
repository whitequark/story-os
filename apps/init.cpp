#include <story.h>

int main()
{
exec("/modules/tty", NULL);
exec("/modules/slsh", NULL);
//exec("/modules/slsh", NULL);
while(1)
 {
 //printf("init: shell died, respawning...\n");
 }
}