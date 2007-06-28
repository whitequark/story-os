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

#include <syscall.h>
#include <hal.h>

//SYSCALLS
/*
 EAX	NAME		REGS				REMARKS
  0	die	ebx = return code
  1	kill	ebx = tid			return code = 1
  2	w/die	ebx = tid to wait		AFTER returning from wait eax will be return code of that tid
  3	w/irq	ebx = irq to wait		need pl = 1, otherwise immediate death
  4	delay	ebx = milliseconds
  5	putc	bl  = char to put
  6	tcolor	bl  = color
  7	palloc	ebx = page count
  8	map	ebx = from ecx = to edx = cnt
*/

unsigned int syscall_die(Registers r)
{
hal->sti();
return hal->taskman->kill(hal->taskman->current->index, r.ebx);
}

unsigned int syscall_kill(Registers r)
{
return hal->taskman->kill(r.ebx, 1);
}

unsigned int syscall_wait_die(Registers r)
{
hal->taskman->current->reason = rsTaskDie;
hal->taskman->current->wait_object = r.ebx;
return 0;
}

unsigned int syscall_wait_irq(Registers r)
{
hal->taskman->current->reason = rsIRQ;
hal->taskman->current->wait_object = r.ebx;
return 0;
}

unsigned int syscall_delay(Registers r)
{
hal->taskman->current->reason = rsDelay;
hal->taskman->current->wait_object = hal->clock->ms_to_ticks(r.ebx);
asm("ljmp $0x30, $0");
return 0;
}

unsigned int syscall_putchar(Registers r)
{
putchar(r.ebx);
return 0;
}

unsigned int syscall_textcolor(Registers r)
{
textcolor(r.ebx);
return 0;
}

unsigned int syscall_morecore(Registers r)
{
unsigned int addr = (unsigned int)hal->taskman->current->vmm->alloc(r.ebx);
return addr;
}

unsigned int syscall_map(Registers r)
{
if(hal->taskman->current->pl > 1)
 return 1;
else
 {
 hal->taskman->current->vmm->map(r.ebx, r.ecx, r.edx, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
 return 0;
 }
}

/*
unsigned int syscall_(Registers r)
{
}
*/

extern "C" unsigned int syscall_handler(Registers r)
{
return hal->syscalls->invoke(r.eax, r);
}

extern "C" void syscall();
asm(
	"_syscall_return_value:\n"
	".long 0\n"

	"syscall:\n"
	"push %ebp\n"
	"pushal\n"

	"push %ds\n"
	"push %es\n"

	"mov  %esp, %ebp\n"

	"pushl %edi\n"
	"pushl %esi\n"
	"pushl %edx\n"
	"pushl %ecx\n"
	"pushl %ebx\n"
	"pushl %eax\n"

	"mov $0x10, %ax\n"
	"mov %ax,%ds\n"
	"mov %ax,%es\n"

	"call syscall_handler\n"
	"mov %eax, _syscall_return_value\n"

	"add $24, %esp\n"

	"pop %es\n"
	"pop %ds\n"

	"popal\n"
	"pop %ebp\n"
	
	"mov _syscall_return_value, %eax\n"

	"iret\n"
);

void SyscallManager::add(unsigned int number, SyscallHandler handler)
{
if(handlers[number])
 hal->panic("Attempt to register existing syscall %i!\n", number);
else
 handlers[number] = handler;
}

unsigned int SyscallManager::invoke(unsigned int number, Registers r)
{
if(handlers[number])
 return handlers[number](r);
else 
 return INVALID_SYSCALL;
}

SyscallManager::SyscallManager()
{
handlers = new SyscallHandler[0x1000];
hal->idt->set_trap(0x31, &syscall, hal->sys_code, 3);

add(0, &syscall_die);
add(1, &syscall_kill);
add(2, &syscall_wait_die);
add(3, &syscall_wait_irq);
add(4, &syscall_delay);
add(5, &syscall_putchar);
add(6, &syscall_textcolor);
add(7, &syscall_morecore);
add(8, &syscall_map);
}
