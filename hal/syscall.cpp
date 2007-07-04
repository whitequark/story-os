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

unsigned int syscall_schedule(Registers r)
{
hal->taskman->schedule();
}

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
handlers = new SyscallHandler[0x100];
hal->idt->set_trap(0x31, &syscall, hal->sys_code, 3);

add(0, &syscall_schedule);
}
