#ifndef _CORE_H_
#define _CORE_H_

#include <multiboot.h>
#include <task.h>
#include <elf.h>
#include <story.h>
#include <messages.h>
#include <interface.h>

class Core
{
private:
Core();
Task* load_elf(unsigned int start, unsigned int size);

public:
Messenger* messenger;
InterfaceManager* interfaces;
Task* load_executable(unsigned int start, unsigned int size, char* command_line);
Core(multiboot_info_t*);
};

extern Core* core;

#endif