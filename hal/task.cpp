//    This file is part of the Story OS
//    Copyright (C) 2007  Peter Zotov
//
//    Story OS is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    Story OS is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. 

#include <hal.h>
#include <task.h>
#include <stdio.h>
#include <assert.h>
#include <colors.h>
#include <stdlib.h>
#include <core.h>

void TaskManager::scheduler()
{
while(1)
 {
 Task* t;
 if(hal->taskman->ticks_remaining <= 0)
  {
  hal->taskman->ticks_remaining = hal->taskman->current->priority;
  unsigned int oldi = hal->taskman->current->index;
  do {  hal->taskman->current = hal->taskman->current->next; }
  while(hal->taskman->current->reason != rsNone || hal->taskman->current->priority == 0);
  unsigned int newi = hal->taskman->current->index;
  #ifdef _DEBUGGING_TASKSWITCHER_
  printf("%z%i>%i%z", YELLOW, oldi, newi, LIGHTGRAY);
  #endif
  hal->taskman->scheduler_running = false;
  hal->taskman->run_task(hal->taskman->current);
  }
 else
  hal->taskman->ticks_remaining--;
 }
}

extern "C" void scheduler_wrapper()
{
hal->taskman->scheduler();
}

extern "C" void timer_handler()
{
static int d;
const int p = 10;
char c[4] = { '\\', '|', '/', '-' };
asm("movb %0, 0xB8000+80*2-1"::"a"(WHITE));
asm("movb %0, 0xB8000+80*2-2"::"a"(c[(d++)/p]));
if(d == p*4) d = 0;

hal->clock->tick();

int n;
Task* t;
for(n = 0, t = hal->taskman->current; t != hal->taskman->current || n == 0; n++, t = t->next)
 if(t->reason == rsDelay)
  {
  t->wait_object--;
  if(t->wait_object <= 0)
   t->reason = rsNone;
  }

hal->outb(0x20, 0x20);

if(!hal->taskman->scheduler_running)
 {
 hal->taskman->scheduler_running = true;
 asm("ljmp $0x30, $0");
 }
}

extern "C" void irq0();
asm(
	"irq0:\n"
	"pusha\n"
	"push %ds\n"
	"push %es\n"

	"mov $0x10, %ax\n"
	"mov %ax,%ds\n"
	"mov %ax,%es\n"

	"movw 40(%esp), %ax\n" /* сохраним cs */
	"push %ax\n"
	"call timer_handler\n"
	"add $2, %esp\n"

	"movb $0x20,%al\n"
	"outb %al,$0x20\n"

	"pop %es\n"
	"pop %ds\n"
	"popa\n"
	"iret\n"
);

bool TaskManager::kill(unsigned int index, unsigned int return_code)
{
hal->cli();

Task *t, *r;
if(index == current->index)
 t = current;
else
 {
 for(t = current->next; t->index != index && t != current; t = t->next); //find task with our index
 if(t == current)
  return false;
 }
if(current->pl > t->pl) 
 return false;

if(return_code != 0)
 core->services->process_kill(t);
core->interfaces->process_kill(t);
//core->messenger->clear();

//find and continue all waiting tasks
for(r = current->next; r != current; r = r->next)
 if(r->reason == rsTaskDie && r->wait_object == index)
  {
  r->reason = rsNone;
  r->tss->eax = return_code;
  }

t->reason = rsDead;

delete t->vmm;

hal->sti();
if(current->index == index)
 asm("ljmp $0x30, $0"); //switch to some other task if this was suicide

return true;
}

void TaskManager::status()
{
Task* t;
int n;
printf("Remaining ticks: %i\n", ticks_remaining);
for(n = 0, t = current; t != current || n == 0; n++, t = t->next)
 {
 printf("Task %i: pl = %i", t->index, t->pl);
 switch(t->reason)
  {
  case rsDead:
  printf(", dead");
  break;
  
  case rsTaskDie:
  printf(", waiting task %i to die", t->wait_object);
  break;
  
  case rsIRQ:
  printf(", waiting IRQ%i to appear", t->wait_object);
  break;
  }
 printf(", priority = %i", t->priority);
 printf("\n");
 }
}

void TaskManager::process_irq(unsigned int number)
{
Task* t;
int n;
for(n = 0, t = current; t != current || n == 0; n++, t = t->next)
 if(t->reason == rsIRQ && t->wait_object == number)
  t->reason = rsNone;
}

Task* TaskManager::create_task(unsigned int pl, unsigned int entry, unsigned int priority, VirtualMemoryManager* vmm)
{
hal->cli_c();

Task* task = new Task;
task->index = next_index++;
task->pl = pl;
task->priority = priority;
task->vmm = vmm;
task->next = current->next;
current->next = task;
current = task;

unsigned int stack_pl0 = (unsigned int) hal->mm->alloc(PL0_STACK_SIZE);
stack_pl0 += PL0_STACK_SIZE * 0x1000 - 1;

task->tss = new TSS;

task->tss->cr3 = task->vmm->get_directory();
task->tss->eip = entry;
task->tss->trace = 0;

task->tss->eflags = 0x202;
task->tss->eax = 0;
task->tss->ebx = 0;
task->tss->ecx = 0;
task->tss->edx = 0;
task->tss->esi = 0;
task->tss->edi = 0;

unsigned int stack_pl3 = (unsigned int) hal->mm->alloc(PL3_STACK_SIZE);
task->vmm->map(stack_pl3, 0xE0000000, PL3_STACK_SIZE, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
stack_pl3 = 0xE0000000 + PL3_STACK_SIZE * 0x1000 - 1;

task->tss->esp0 = stack_pl0;
task->tss->esp = stack_pl3;
task->tss->ebp = stack_pl3;

task->tss->cs = hal->app_code;
task->tss->es = hal->app_data;
task->tss->ss = hal->app_data;
task->tss->ds = hal->app_data;
task->tss->ss0 = hal->sys_data;

task->tss->fs = 0;
task->tss->gs = 0;
task->tss->ldt = 0;
task->tss->iomap = 0;

task->message = NULL;

task->descriptor = new TSSDescriptor((unsigned int)task->tss);

hal->sti_c();
return task;
}

TaskManager::TaskManager()
{
next_index = 0;
ticks_remaining = 0;
scheduler_running = false;

hal->idt->set_interrupt(0x20, &irq0, hal->sys_code);

current = new Task; //kernel task
current->index = next_index++;
current->pl = 0;
current->priority = 0; //do not execute at all
current->next = current;

current->tss = new TSS;

current->tss->cr3 = (u32) hal->pagedir;
current->tss->trace = 0;
current->tss->iomap = 0;
current->tss->ldt = 0;

current->tss->cs = hal->sys_code;
current->tss->ds = hal->sys_data;
current->tss->ss = hal->sys_data;
current->tss->es = hal->sys_data;

app_tss = hal->gdt->add_descriptor(new TSSDescriptor((unsigned int) current->tss));
load_tr(app_tss);

//SCHEDULER

Task* task = new Task;
task->index = next_index++;
task->pl = 0;
task->priority = 0;
task->vmm = NULL;
task->next = current->next;
current->next = task;

unsigned int stack_pl0 = (unsigned int) hal->mm->alloc(PL0_STACK_SIZE);
stack_pl0 += PL0_STACK_SIZE * 0x1000 - 1;

task->tss = new TSS;

task->tss->cr3 = (unsigned int)hal->pagedir;
task->tss->eip = (unsigned int)&scheduler_wrapper;
task->tss->trace = 0;

task->tss->eflags = 0x202;
task->tss->eax = 0;
task->tss->ebx = 0;
task->tss->ecx = 0;
task->tss->edx = 0;
task->tss->esi = 0;
task->tss->edi = 0;

task->tss->esp = stack_pl0;
task->tss->ebp = stack_pl0;

task->tss->cs = hal->sys_code;
task->tss->es = hal->sys_data;
task->tss->ss = hal->sys_data;
task->tss->ds = hal->sys_data;

task->tss->fs = 0;
task->tss->gs = 0;
task->tss->ldt = 0;
task->tss->iomap = 0;

sched_tss = hal->gdt->add_descriptor(new TSSDescriptor((unsigned int) task->tss));
}

void TaskManager::load_tr(unsigned short descriptor)
{
asm("ltr %0"::"a"((unsigned short)(descriptor*8)));
}

void TaskManager::run_task(Task* task)
{
hal->gdt->modify_descriptor(task->descriptor, app_tss);
asm("ljmp $0x28, $0");
}

Task* TaskManager::task(unsigned int index)
{
Task* tsk;
for(tsk = current->next; (tsk != current) && (tsk->index != index); tsk = tsk->next);
if(tsk->index == index) 
 return tsk;
else
 return NULL;
}