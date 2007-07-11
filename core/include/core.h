#ifndef _CORE_H_
#define _CORE_H_

#include <multiboot.h>
#include <task.h>
#include <elf.h>
#include <story.h>
#include <messages.h>
#include <list.h>

void process_manager();

class Core
{
private:
Core();
Task* load_elf(unsigned int start, unsigned int size);
Task* procman;
List<unsigned int>* IRQs[16];

public:
CoreMessenger* messenger;
bool procman_initialized;

Task* load_executable(unsigned int start, unsigned int size, char* command_line);
Core(multiboot_info_t*);
void launch_procman();
void process_irq(unsigned int number);
void attach_irq(unsigned int irq, unsigned int task);
};

extern Core* core;

#endif