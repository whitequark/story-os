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
#include <assert.h>
#include <colors.h>
#include <stdlib.h>
#include <core.h>

void TaskManager::scheduler()
{
while(1)
 {
 Task* t;
 if(ticks_remaining <= 0)
  {
  ticks_remaining = current->priority;
  unsigned int oldi = current->index;
  do 
   {
   current = current->next; 
   if(current->wait_reason == wrNotNULL)
    if(*((unsigned int*) current->wait_object) != 0)
     current->wait_reason = wrNone;
   }
  while(current->wait_reason != wrNone || current->priority == 0);
  unsigned int newi = current->index;
  //printf("%z%i>%i%z ", YELLOW, oldi, newi, LIGHTGRAY);
  scheduler_running = false;
  run_task(current);
  }
 else
  ticks_remaining--;
 }
}

extern "C" void scheduler_wrapper()
{
hal->taskman->scheduler();
}

extern "C" void timer_handler()
{
static int d;
const int p = 300;
if(d == p*4) d = 0;
/*if(d > p * 2)
 { hal->outb(0x0a, 0x3d4); hal->outb(12, 0x3d5);
   hal->outb(0x0b, 0x3d4); hal->outb(13, 0x3d5); } 
else 
 { hal->outb(0x0a, 0x3d4); hal->outb(32, 0x3d5);
   hal->outb(0x0b, 0x3d4); hal->outb(32, 0x3d5); }*/

hal->clock->tick();

int n;
Task* t;
for(n = 0, t = hal->taskman->current; t != hal->taskman->current || n == 0; n++, t = t->next)
 if(t->wait_reason == wrDelay)
  {
  t->wait_object--;
  if(t->wait_object == 0)
   t->wait_reason = wrNone;
  }

hal->outb(0x20, 0x20);

if(!hal->taskman->scheduler_running && !hal->taskman->no_schedule)
 {
 hal->taskman->scheduler_running = true;
 hal->taskman->schedule();
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

	"movw 40(%esp), %ax\n" /* �������� cs */
	"push %ax\n"
	"call timer_handler\n"
	"add $2, %esp\n"

	"pop %es\n"
	"pop %ds\n"
	"popa\n"
	"iret\n"
);

bool TaskManager::kill(unsigned int index, unsigned int return_code)
{
Task *t, *r;
if(index == current->index)
 t = current;
else
 for(t = current->next; t->index != index && t != current; t = t->next); //find task with our index
if(t->index != index) 
 return false;

t->wait_reason = wrDead;

if(t->vmm->change_threads(-1) == 0)
 delete t->vmm;

return true;
}

void TaskManager::process_irq(unsigned int number)
{
core->process_irq(number);
}

Task* TaskManager::create_task(unsigned int pl, unsigned int entry, unsigned int priority, VirtualMemoryManager* vmm, unsigned int push, unsigned int* data)
{
hal->cli_c();
Task* task = new Task;
task->index = next_index++;
task->pl = pl;
task->priority = priority;
task->vmm = vmm;
task->next = current->next;
current->next = task;

unsigned int stack_pl0 = (unsigned int) hal->mm->alloc(PL0_STACK_SIZE);
stack_pl0 += PL0_STACK_SIZE * 0x1000 - 4;

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

unsigned int stack_pl3 = (unsigned int) task->vmm->alloc(PL3_STACK_SIZE, "stack");
stack_pl3 = stack_pl3 + PL3_STACK_SIZE * 0x1000 - 4;
for(int i = 0; i < push; i++)
 *((unsigned int*)vmm->virtual_to_physical(stack_pl3 - i * 4)) = data[i];
stack_pl3 -= push * 4;

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

task->messages = NULL;
task->reply = NULL;

task->descriptor = new TSSDescriptor((unsigned int)task->tss);
hal->sti_c();

return task;
}

void TaskManager::status()
{
Task* t;
int n;
printf("Remaining ticks: %i\n", ticks_remaining);
for(n = 0, t = current; t != current || n == 0; n++, t = t->next)
 {
 printf("Task %i: pl = %i", t->index, t->pl);
 switch(t->wait_reason)
  {
  case wrNone:
  printf(", ready to run");
  break;
  
  case wrDead:
  printf(", dead");
  break;
  
  case wrTaskDie:
  printf(", waiting task %i to die", t->wait_object);
  break;
  
  case wrDelay:
  printf(", waiting %i milliseconds", t->wait_object);
  break;
  
  case wrMessage:
  printf(", waiting for message");
  break;
  
  case wrReply:
  printf(", waiting for reply");
  break;
  
  case wrNotNULL:
  printf(", waiting for 0x%X became not null", t->wait_object);
  break;
  
  default:
  printf(", waiting unknown object");
  }
 printf(", priority = %i", t->priority);
 printf("\n");
 }
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
current->wait_reason = wrDead;

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
task->next = current;
task->wait_reason = wrDead;
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

void TaskManager::schedule()
{
asm("ljmp $0x30, $0");
}