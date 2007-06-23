#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include <task.h>

typedef struct SFunction
{
char* name;
char* parameters;
SFunction* next;
} Function;

typedef struct SInterface
{
char* name;
unsigned int task;
Function* function;
SInterface* next;
SInterface* prev;
} Interface;

class InterfaceManager
{
private:
Interface* interface;

public:
InterfaceManager();
void process_kill(Task* task);
bool add(char* name);
Interface* get(char* name);
bool function_add(char* fname, char* iname, char* parameters);
Function* function_get(char* fname, char* iname);
Function* function_get(char* fname, char* iname, char* parameters);
};

#endif