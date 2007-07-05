#ifndef _CORE_H_
#define _CORE_H_

#include <multiboot.h>
#include <task.h>
#include <elf.h>
#include <story.h>
#include <messages.h>

void process_manager();

class Core
{
private:
Core();
Task* load_elf(unsigned int start, unsigned int size);
Task* procman;

public:
CoreMessenger* messenger;
Task* load_executable(unsigned int start, unsigned int size, char* command_line);
Core(multiboot_info_t*);
void launch_procman();
};

extern Core* core;

#endif