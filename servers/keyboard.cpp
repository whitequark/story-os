#include <story.h>
#include <file.h>
#include <service.h>

int keyboard_thread()
{
gain_io_privilegies();
attach_irq(1);
while(inb(0x64) & 1)
 inb(0x60);
outb(0x61, 0xFF);
printf("keyboard: initialized\n");

while(1)
 {
 while(!(inb(0x64) & 1));
 unsigned char scancode = inb(0x60);
 printf("%c", scancode);
 }
}

int main()
{
unsigned int thread = start_thread(&keyboard_thread);
}