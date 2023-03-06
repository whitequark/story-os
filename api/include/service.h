#ifndef _SERVICE_H_
#define _SERVICE_H_

void inline outb(unsigned short port, unsigned char value)
{ asm volatile("outb %b0,%w1":: "a"(value), "d"(port)); }

unsigned char inline inb(unsigned short port)
{ char value; asm volatile("inb %w1, %b0": "=a"(value): "d"(port)); return value; }

void inline outw(unsigned short port, unsigned short value)
{ asm volatile("outw %%ax, %%dx"::"a"(value),"d"(port)); }

unsigned short inline inw(unsigned short port)
{ short value; asm volatile("inw %%dx, %%ax":"=a"(value):"d"(port)); return value; }

int gain_io_privilegies();
void attach_irq(unsigned int irq);
void* attach_memory(unsigned int count, unsigned int physical);

#endif
