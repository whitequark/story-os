#ifndef _INTERFACE_H_
#define _INTERFACE_H_

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
Interface* get(char* name);
};

#endif