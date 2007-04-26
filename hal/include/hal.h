#ifndef _HAL_H_
#define _HAL_H_

#include <multiboot.h>
#include <terminal_driver.h>
#include <gdt.h>
#include <stubs.h>
#include <idt.h>
#include <pic.h>
#include <mm.h>
#include <task.h>
#include <syscall.h>
#include <clock.h>
#include <paging.h>

class HAL
{
private:
int int_block_count;
multiboot_info_t mbi;

HAL();

public:
char* lfb;

TerminalDriver* terminal;
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

void outb(unsigned short port, unsigned char value);
unsigned char inb(unsigned short port);
void outw(unsigned short port, unsigned short value);
unsigned short inw(unsigned short port);

void cli();
void sti();
void cli_c(); //with counter
void sti_c();

void panic(char*, ...);
void failure(char*);

HAL(multiboot_info_t* mbi);
};

extern HAL* hal;

void putchar(char c);
void textcolor(char color);
void* palloc(unsigned int count);
void pfree(void* address);

#endif
