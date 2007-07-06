#ifndef _TASK_H_
#define _TASK_H_

#include <story.h>
#include <vmm.h>
#include <gdt.h>
#include <messages.h>

#define PL0_STACK_SIZE 2
#define PL3_STACK_SIZE 16

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef struct 
{
u16	backlink, __blh;
u32	esp0;
u16	ss0, __ss0h;
u32	esp1;
u16	ss1, __ss1h;
u32	esp2;
u16	ss2, __ss2h;
u32	cr3;
u32	eip;
u32	eflags;
u32	eax, ecx, edx, ebx;
u32	esp, ebp, esi, edi;
u16	es, __esh;
u16	cs, __csh;
u16	ss, __ssh;
u16	ds, __dsh;
u16	fs, __fsh;
u16	gs, __gsh;
u16	ldt, __ldth;
u16	trace, iomap;
} __attribute__((__packed__)) TSS;

typedef struct SRegisters
{
unsigned int   eax;
unsigned int   ebx;
unsigned int   ecx;
unsigned int   edx;
unsigned int   esi;
unsigned int   edi;
} __attribute__((__packed__)) Registers;

typedef enum { rsNone, rsDead, rsTaskDie, rsIRQ, rsDelay, rsMessage, rsReply } WaitingReason;

/*
PL's:
 0   = Kernel
 1   = System
 2   = Root
 3-x = User
*/

class Task
{
public:
Task* next;

TSS* tss;
TSSDescriptor* descriptor;

unsigned int index;

unsigned int pl; //privilegy level
unsigned int priority;

void* image;
VirtualMemoryManager* vmm;

CoreMessage* message;
CoreMessage* reply;

WaitingReason reason;
unsigned int wait_object;
};

extern "C" void timer_handler();

class TaskManager
{
friend void timer_handler();

private:
int next_index;
unsigned int ticks_remaining;
unsigned short app_tss;
unsigned short sched_tss;
void load_tr(unsigned short descriptor);
bool scheduling;
void run_task(Task* task);

public:
Task* current;
TaskManager();
bool kill(unsigned int index, unsigned int return_code = 1); // 0=OK 1=GenericError 2-x=User accessible
void process_irq(unsigned int number);
void scheduler();
Task* create_task(unsigned int pl, unsigned int entry, unsigned int priority, VirtualMemoryManager* vmm);
bool scheduler_running;
Task* task(unsigned int index);
void schedule();
void status();
};

#endif