#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include <task.h>

typedef struct SInterface
{
char* name;
unsigned int task;
SInterface* next;
} Interface;

class InterfaceManager
{
private:
Interface* interface;

public:
InterfaceManager();
bool add(char* name);
void process_kill(Task* task);
Interface* get(char* name);
};

#endif