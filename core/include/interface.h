#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include <task.h>

struct CoreInterface
{
char* name;
unsigned int task;
CoreInterface* next;
CoreInterface* prev;
};

class InterfaceManager
{
private:
CoreInterface* interface;

public:
InterfaceManager();
void process_kill(Task* task);
bool add(char* name);
CoreInterface* get(char* name);
};

#endif