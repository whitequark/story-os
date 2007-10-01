#ifndef _HAL_H_
#define _HAL_H_

#include <multiboot.h>
#include <kprintf.h>
#include <gdt.h>
#include <stubs.h>
#include <idt.h>
#include <pic.h>
#include <mm.h>
#include <task.h>
#include <syscall.h>
#include <clock.h>
#include <paging.h>

class KernelTerminal
{
private:
int cursorx;
int cursory;
unsigned short *lfb;
unsigned char color;

public:
KernelTerminal();

void put_char(char ch);
void put_char_raw(unsigned char ch, unsigned char color);
void set_color(unsigned char color);
void clear();
};

class HAL
{
private:
int int_block_count;

HAL();
void _backtrace();

public:
char* lfb;
multiboot_info_t *mbi;

KernelTerminal* terminal;
MemoryManager* mm;
GDT* gdt;
IDT* idt;
PIC* pic;
TaskManager* taskman;
SyscallManager* syscalls;
Clock* clock;
Paging* paging;

unsigned short sys_code;
unsigned short sys_data;
unsigned short app_code;
unsigned short app_data;

PageDirectory* pagedir;

unsigned int get_memory_amount();

void wait_for_debugger();

char* option(char* opt_needed);

void outb(unsigned short port, unsigned char value)
{ asm volatile("outb %b0,%w1":: "a"(value), "d"(port)); }

unsigned char inb(unsigned short port)
{ char value; asm volatile("inb %w1, %b0": "=a"(value): "d"(port)); return value; }

void outw(unsigned short port, unsigned short value)
{ asm volatile("outw %%ax, %%dx"::"a"(value),"d"(port)); }

unsigned short inw(unsigned short port)
{ short value; asm volatile("inw %%dx, %%ax":"=a"(value):"d"(port)); return value; }

void cli();
void sti();
void cli_c(); //with counter
void sti_c();

void panic(char*, ...);

HAL(multiboot_info_t* mbi);
};

extern HAL* hal;

void kernel_backtrace();
void user_backtrace();
void raw_backtrace();

void putchar(char c);
void textcolor(char color);

#endif
