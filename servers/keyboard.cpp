#include <story.h>
#include <file.h>

void keyboard_thread();

int main()
{
printf("launched %i\n", start_thread(&keyboard_thread));
}

void keyboard_thread()
{
printf("i am alive!!!\n");
while(1);
}