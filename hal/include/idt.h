#ifndef _IDT_H_
#define _IDT_H_

#define IRQ_HANDLER(func) extern "C" void func (); void _i_ ## func () { asm(".globl " #func " \n " #func ": cli \n pusha \n call _" #func " \n movb $0x20, %al \n outb %al, $0x20 \n popa \n sti \n iret \n"); } extern "C" void _ ## func(void)

//when irq>0x7 we need EOI both slave&master
#define IRQ_HANDLER_HIGH(func) extern "C" void func (); void _i_ ## func () { asm(".globl " #func " \n " #func ": cli \n pusha \n call _" #func " \n movb $0x20, %al \n outb %al, $0x20 \n outb %al, $0xA0 \n popa \n sti \n iret \n"); } extern "C" void _ ## func(void)

#define EXCEPTION_HANDLER(func) \
 extern "C" void func (unsigned int errcode); \
 asm( \
 ".globl " #func "\n" \
 #func ":\n" \
 "popl " #func  "_errcode \n" \
 "popl " #func "_address \n" \
 "pushl " #func "_address \n" \
 "pusha \n" \
 "pushl " #func "_errcode \n" \
 "pushl " #func "_address \n" \
 "call _" #func "\n" \
 "addl $8, %esp \n" \
 "popa \n" \
 "iret \n" \
 #func "_errcode: .long 0 \n" \
 #func "_address: .long 0"); \
 extern "C" void _ ## func(unsigned int address, unsigned int errcode)

#define MAKE_ISR(x) ((void(*)())x)
 
#include <gdt.h>
 
class IDT
{
private:
char idt[256*8]; //it's enough for all!

public:
IDT();
void set_trap(unsigned char number, void (*handler) (), unsigned short selector, unsigned char dpl = 0);
void set_interrupt(unsigned char number, void (*handler) (), unsigned short code_selector);
void install();
void register_exceptions();
void register_irqs();
};

#endif