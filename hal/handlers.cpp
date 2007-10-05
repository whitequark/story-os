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

#include <idt.h>
#include <hal.h>
#include <colors.h>

#define SAFE_CURRENT_TASK hal->taskman ? hal->taskman->current ? hal->taskman->current->index : 0 : 0

extern "C" void handle_page_fault(unsigned int address, unsigned int errcode, unsigned int eip)
{
printf("\n Unresolved Page Fault (task %d)\n  Cannot %s 0x%X (%s) at level %d\n  EIP: %X\n", 
          SAFE_CURRENT_TASK,
          errcode & 2 ? "write" : "read",
          address,
          errcode & 1 ? "permission denied" : "page not found",
          errcode & 4 ? 3 : 0,
          eip);
kernel_backtrace();
}

extern "C" void handle_general_protection_fault(unsigned int errcode, unsigned int eip)
{
if(errcode == 0)
 printf("\n General Protection Fault (task %d)\n  EIP: %X\n", 
          SAFE_CURRENT_TASK, 
          eip);
else
 printf("\n General Protection Fault (task %d)\n  EIP: %X\n", 
          SAFE_CURRENT_TASK, 
          eip); 
}

extern "C" void handle_segment_not_present(unsigned int errcode, unsigned int eip)
{
printf("\n Segment Not Present (task %d)\n  Event: %s; selector: %X (in %s)\n  EIP: %X\n", 
          SAFE_CURRENT_TASK,
          errcode & 1 ? "external" : "internal",
          errcode & 0xFFFC,
          errcode & 2 ? "IDT" : "GDT",
          eip);
}

extern "C" void handle_stack_fault(unsigned int errcode, unsigned int eip)
{
if(errcode == 0)
 printf("\n Stack Fault (task %d)\n  Stack Limit Exceeded\n  EIP: %X\n", 
          SAFE_CURRENT_TASK,
          eip);
else
 printf("\n Stack Fault (task %d)\n  Selector: %X\n  EIP: %X\n", 
          SAFE_CURRENT_TASK,
          errcode,
          eip);
}

extern "C" void exception0();
extern "C" void exception1();
extern "C" void exception2();
extern "C" void exception3();
extern "C" void exception4();
extern "C" void exception5();
extern "C" void exception6();
extern "C" void exception7();
extern "C" void exception8();
extern "C" void exception9();
extern "C" void exceptionA();
extern "C" void exceptionB();
extern "C" void exceptionC();
extern "C" void exceptionD();
extern "C" void exceptionE();
extern "C" void exception10();
extern "C" void exception11();
extern "C" void exception12();
extern "C" void exception13();
extern "C" void exception_unknown();

asm( //#DE Division By Zero: fault, no errcode
 ".globl exception0\n"
 "exception0:\n"
 "cli\n"
 "movb $0x30, 0xb8000\n"
 "movb $0x04, 0xb8001\n"
 "loop0: jmp loop0\n"
 );

asm( //#DB Debug: trap/fault (see IA manual)
 ".globl exception1\n"
 "exception1:\n"
 "cli\n"
 "movb $0x31, 0xb8000\n"
 "movb $0x04, 0xb8001\n"
 "loop1: jmp loop1\n"
 );

asm( //NMI interrupt, really not an exception
 ".globl exception2\n"
 "exception2:\n"
 "cli\n"
 "movb $0x32, 0xb8000\n"
 "movb $0x04, 0xb8001\n"
 "loop2: jmp loop2\n"
 );

asm( //#BP Breakpoint: trap, no errcode
 ".globl exception3\n"
 "exception3:\n"
 "cli\n"
 "movb $0x33, 0xb8000\n"
 "movb $0x04, 0xb8001\n"
 "loop3: jmp loop3\n"
 );

asm( //#OF Owerflow: trap, no errcode
 ".globl exception4\n"
 "exception4:\n"
 "cli\n"
 "movb $0x34, 0xb8000\n"
 "movb $0x04, 0xb8001\n"
 "loop4: jmp loop4\n"
 );

asm( //#BR Bound Range Exceeded, fault, no errcode
 ".globl exception5\n"
 "exception5:\n"
 "cli\n"
 "movb $0x35, 0xb8000\n"
 "movb $0x04, 0xb8001\n"
 "loop5: jmp loop5\n"
 );

asm( //#UD Invalid Opcode, fault, no errcode
 ".globl exception6\n"
 "exception6:\n"
 "cli\n"
 "movb $0x36, 0xb8000\n"
 "movb $0x04, 0xb8001\n"
 "loop6: jmp loop6\n"
 );

asm( //#NM Device Not avaliable, fault, no errcode
 ".globl exception7\n"
 "exception7:\n"
 "cli\n"
 "movb $0x37, 0xb8000\n"
 "movb $0x04, 0xb8001\n"
 "loop7: jmp loop7\n"
 );

asm( //#DF Double Fault, abort, errcode = 0
 ".globl exception8\n"
 "exception8:\n"
 "cli\n"
 "movb $0x38, 0xb8000\n"
 "movb $0x04, 0xb8001\n"
 "loop8: jmp loop8\n"
 );

asm( //Reserved, abort, no errcode
 ".globl exception9\n"
 "exception9:\n"
 "cli\n"
 "movb $0x39, 0xb8000\n"
 "movb $0x04, 0xb8001\n"
 "loop9: jmp loop9\n"
 );

asm( //#TS Invalid TSS, fault, errcode = selector
 ".globl exceptionA\n"
 "exceptionA:\n"
 "cli\n"
 "movb $65, 0xb8000\n"
 "movb $0x04, 0xb8001\n"
 "loopA: jmp loopA\n"
 );

asm( //#NP Segment Not Present, fault, errcode = selector
 ".globl exceptionB\n"
 "exceptionB:\n"
 "cli\n"
 "movb $66, 0xb8000\n"
 "movb $0x04, 0xb8001\n"
 "call handle_segment_not_present\n"
 "loopB: jmp loopB\n"
 );

asm( //#SS Stack Fault, fault, errcode = { 0 => limit violation, !0 => selector }
 ".globl exceptionC\n"
 "exceptionC:\n"
 "cli\n"
 "movb $67, 0xb8000\n"
 "movb $0x04, 0xb8001\n"
 "call handle_stack_fault\n"
 "loopC: jmp loopC\n"
 );

asm( //#GP General Protection Fault, fault, errcode = { !0 => loading selector, 0 => * }
 ".globl exceptionD\n"
 "exceptionD:\n"
 "cli\n"
 "movb $68, 0xb8000\n"
 "movb $0x04, 0xb8001\n"
 "call handle_general_protection_fault\n"
 "loopD: jmp loopD\n"
 );

asm( //#PF Page Fault, fault, errcode = see manual, cr2 = linear address
 ".globl exceptionE\n"
 "exceptionE:\n"
 "cli\n"
 "movb $69, 0xb8000\n"
 "movb $0x04, 0xb8001\n"
 "movl %cr2, %eax\n"
 "pushl %eax\n"
 "call handle_page_fault\n"
 "loopE: jmp loopE\n"
 );

asm( //#MF Floating Point Error, fault, no errcode
 ".globl exception10\n"
 "exception10:\n"
 "cli\n"
 "movb $70, 0xb8000\n"
 "movb $0x04, 0xb8001\n"
 "loop10: jmp loop10\n"
 );

asm( //#AC Alignment Check, fault, errcode = 0
 ".globl exception11\n"
 "exception11:\n"
 "cli\n"
 "movb $0x31, 0xb8000\n"
 "movb $0x04, 0xb8001\n"
 "movb $0x31, 0xb8002\n"
 "movb $0x04, 0xb8003\n"
 "loop11: jmp loop11\n"
 );

asm( //#MC Machine Check, abort, no errcode
 ".globl exception12\n"
 "exception12:\n"
 "cli\n"
 "movb $0x31, 0xb8000\n"
 "movb $0x04, 0xb8001\n"
 "movb $0x32, 0xb8002\n"
 "movb $0x04, 0xb8003\n"
 "loop12: jmp loop12\n"
 );

asm( //#XF, SIMD Floating Point Exception, fault, no errcode
 ".globl exception13\n"
 "exception13:\n"
 "cli\n"
 "movb $0x31, 0xb8000\n"
 "movb $0x04, 0xb8001\n"
 "movb $0x33, 0xb8002\n"
 "movb $0x04, 0xb8003\n"
 "loop13: jmp loop13\n"
 );

asm(
 ".globl exception_unknown\n"
 "exception_unknown:\n"
 "cli\n"
 "movb $0x1, 0xb8000\n"
 "movb $0x04, 0xb8001\n"
 "loop_u: jmp loop_u\n"
 );

IRQ_HANDLER(irq0_handler) { printf("."); }
IRQ_HANDLER(irq1_handler) { hal->taskman->process_irq(1);  }
IRQ_HANDLER(irq2_handler) { hal->taskman->process_irq(2);  }
IRQ_HANDLER(irq3_handler) { hal->taskman->process_irq(3);  }
IRQ_HANDLER(irq4_handler) { hal->taskman->process_irq(4);  }
IRQ_HANDLER(irq5_handler) { hal->taskman->process_irq(5);  }
IRQ_HANDLER(irq6_handler) { hal->taskman->process_irq(6);  }
IRQ_HANDLER(irq7_handler) { /*hal->taskman->process_irq(7);*/  }
IRQ_HANDLER(irq8_handler) { hal->taskman->process_irq(8);  }
IRQ_HANDLER(irq9_handler) { hal->taskman->process_irq(9);  }
IRQ_HANDLER(irqa_handler) { hal->taskman->process_irq(10); }
IRQ_HANDLER(irqb_handler) { hal->taskman->process_irq(11); }
IRQ_HANDLER(irqc_handler) { hal->taskman->process_irq(12); }
IRQ_HANDLER(irqd_handler) { hal->taskman->process_irq(13); }
IRQ_HANDLER(irqe_handler) { hal->taskman->process_irq(14); }
IRQ_HANDLER(irqf_handler) { hal->taskman->process_irq(15); }

void IDT::register_exceptions()
{
set_trap(0x00, MAKE_ISR(exception0), hal->sys_code);
set_trap(0x01, MAKE_ISR(exception1), hal->sys_code);
set_trap(0x02, MAKE_ISR(exception2), hal->sys_code);
set_trap(0x03, MAKE_ISR(exception3), hal->sys_code, 3);
set_trap(0x04, MAKE_ISR(exception4), hal->sys_code, 3);
set_trap(0x05, MAKE_ISR(exception5), hal->sys_code, 3);
set_trap(0x06, MAKE_ISR(exception6), hal->sys_code);
set_trap(0x07, MAKE_ISR(exception7), hal->sys_code);
set_trap(0x08, MAKE_ISR(exception8), hal->sys_code);
set_trap(0x09, MAKE_ISR(exception9), hal->sys_code);
set_trap(0x0a, MAKE_ISR(exceptionA), hal->sys_code);
set_trap(0x0b, MAKE_ISR(exceptionB), hal->sys_code);
set_trap(0x0c, MAKE_ISR(exceptionC), hal->sys_code);
set_trap(0x0d, MAKE_ISR(exceptionD), hal->sys_code);
set_trap(0x0e, MAKE_ISR(exceptionE), hal->sys_code);
set_trap(0x10, MAKE_ISR(exception10), hal->sys_code);
set_trap(0x11, MAKE_ISR(exception11), hal->sys_code);
set_trap(0x12, MAKE_ISR(exception12), hal->sys_code);
set_trap(0x13, MAKE_ISR(exception13), hal->sys_code);
set_trap(0x14, MAKE_ISR(exception_unknown), hal->sys_code);
set_trap(0x15, MAKE_ISR(exception_unknown), hal->sys_code);
set_trap(0x16, MAKE_ISR(exception_unknown), hal->sys_code);
set_trap(0x17, MAKE_ISR(exception_unknown), hal->sys_code);
set_trap(0x18, MAKE_ISR(exception_unknown), hal->sys_code);
set_trap(0x19, MAKE_ISR(exception_unknown), hal->sys_code);
}

void IDT::register_irqs()
{
int i;
set_trap(0x20, irq0_handler, hal->sys_code);
set_trap(0x21, irq1_handler, hal->sys_code);
set_trap(0x22, irq2_handler, hal->sys_code);
set_trap(0x23, irq3_handler, hal->sys_code);
set_trap(0x24, irq4_handler, hal->sys_code);
set_trap(0x25, irq5_handler, hal->sys_code);
set_trap(0x26, irq6_handler, hal->sys_code);
set_trap(0x27, irq7_handler, hal->sys_code);

set_trap(0x28, irq8_handler, hal->sys_code);
set_trap(0x29, irq9_handler, hal->sys_code);
set_trap(0x2a, irqa_handler, hal->sys_code);
set_trap(0x2b, irqb_handler, hal->sys_code);
set_trap(0x2c, irqc_handler, hal->sys_code);
set_trap(0x2d, irqd_handler, hal->sys_code);
set_trap(0x2e, irqe_handler, hal->sys_code);
set_trap(0x2f, irqf_handler, hal->sys_code);
}