#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <task.h>

#define INVALID_SYSCALL 0xFFFFFFFF

typedef unsigned int (* SyscallHandler) (Registers);

class SyscallManager
{
private:
SyscallHandler* handlers;

public:
void add(unsigned int number, SyscallHandler handler);
unsigned int invoke(unsigned int number, Registers r);
SyscallManager();
};

#endif