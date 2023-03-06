#ifndef _CORE_H_
#define _CORE_H_

#include <multiboot.h>
#include <task.h>
#include <elf.h>
#include <story.h>
#include <list.h>

void process_manager();

class Core
{
private:
Core();
Task* load_elf(unsigned int start, unsigned int size);
Task *procman, *idle;
List<unsigned int>* IRQs[16];
Messenger* messenger;

public:
Task* load_executable(unsigned int start, unsigned int size, char* command_line);
Core(multiboot_info_t*);
void process_irq(unsigned int number);
void attach_irq(unsigned int irq, unsigned int task);
void launch_procman();
void launch_idle();
};

extern Core* core;

#endif
