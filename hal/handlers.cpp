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

void register_exceptions();

const char* exception_names[20] = {
"Division By Zero",
"Debug",
"Non Maskable Interrupt",
"Breakpoint",
"Into Detected Overflow",
"Out of Bounds",
"Invalid Opcode",
"No Coprocessor",
"Double Fault",
"Coprocessor Segment Overrun",
"Bad TSS Exception",
"Segment Not Present Exception",
"Stack Fault Exception",
"General Protection Fault",
"Page Fault Exception",
"Unknown Interrupt Exception",
"Coprocessor Fault Exception",
"Alignment Check",
"Machine Check",
"Unknown Exception"
};

void common_exception_handler(int number, int address, int errcode, HRegisters r)
{
hal->cli_c();
unsigned int cr2;
asm("mov %%cr2, %0":"=d"(cr2));
printf("\n    %zException Occured (Task %i)!%z", YELLOW, hal->taskman->current->index, LIGHTGRAY); 
printf(" EIP: %z%X%z", LIGHTGREEN, address, LIGHTGRAY);
printf("\n    Error:%z ", LIGHTBLUE);
printf(exception_names[number]);
printf(" %zError Code:%z ", LIGHTGRAY, LIGHTBLUE);
printf("%X%z", errcode, LIGHTGRAY);
if(number == 0x0E)
 {
 if(errcode & 2)	printf("\n    %zWrite%z error: ", LIGHTRED, LIGHTGRAY);
 else			printf("\n    %zRead%z error: ", LIGHTGREEN, LIGHTGRAY);
 if(errcode & 1)	printf("%zpermission denied%z", YELLOW, LIGHTGRAY);
 else			printf("%zpage not found%z", YELLOW, LIGHTGRAY);
 if(errcode & 4)	printf(" at level %z3%z, ", LIGHTBLUE, LIGHTGRAY);
 else			printf(" at level %z0%z, ", LIGHTBLUE, LIGHTGRAY);
 printf("address %z%X%z", LIGHTGREEN, cr2, LIGHTGRAY);
 printf("\n    ESP: %X", r.esp);
 }
if(hal->taskman->current->index == 0)
 {
 printf("\n      %zSystem halted.\n\n", LIGHTRED);
 kernel_backtrace();
 for(;;);
 }
else
 {
 if(number == 0x0E)
  printf("\n      %zTask will be terminated.%z\n\n", LIGHTRED, LIGHTGRAY);
 else
  printf("\n      %zSystem halted.%z\n\n", LIGHTRED, LIGHTGRAY);
 hal->taskman->current->vmm->show();
 if(number == 0x0E)
  {
  kernel_backtrace();
  /*int index = hal->taskman->current->index;
  hal->taskman->kill(hal->taskman->current->index, 1);
  hal->sti_c();*/
  }
 while(1);
 }
}

EXCEPTION_HANDLER(exception0) { common_exception_handler(0, address, errcode, r); }
EXCEPTION_HANDLER(exception1) { common_exception_handler(1, address, errcode, r); }
EXCEPTION_HANDLER(exception2) { common_exception_handler(2, address, errcode, r); }
EXCEPTION_HANDLER(exception3) { common_exception_handler(3, address, errcode, r); }
EXCEPTION_HANDLER(exception4) { common_exception_handler(4, address, errcode, r); }
EXCEPTION_HANDLER(exception5) { common_exception_handler(5, address, errcode, r); }
EXCEPTION_HANDLER(exception6) { common_exception_handler(6, address, errcode, r); }
EXCEPTION_HANDLER(exception7) { common_exception_handler(7, address, errcode, r); }
EXCEPTION_HANDLER(exception8) { common_exception_handler(8, address, errcode, r); }
EXCEPTION_HANDLER(exception9) { common_exception_handler(9, address, errcode, r); }
EXCEPTION_HANDLER(exceptionA) { common_exception_handler(10, address, errcode, r); }
EXCEPTION_HANDLER(exceptionB) { common_exception_handler(11, address, errcode, r); }
EXCEPTION_HANDLER(exceptionC) { common_exception_handler(12, address, errcode, r); }
EXCEPTION_HANDLER(exceptionD) { common_exception_handler(13, address, errcode, r); }
EXCEPTION_HANDLER(exceptionE) { common_exception_handler(14, address, errcode, r); }
EXCEPTION_HANDLER(exceptionF) { common_exception_handler(15, address, errcode, r); }
EXCEPTION_HANDLER(exception10) { common_exception_handler(16, address, errcode, r); }
EXCEPTION_HANDLER(exception11) { common_exception_handler(17, address, errcode, r); }
EXCEPTION_HANDLER(exception12) { common_exception_handler(18, address, errcode, r); }
EXCEPTION_HANDLER(exception_unknown) { common_exception_handler(19, address, errcode, r); }

IRQ_HANDLER(irq0_handler)      { hal->taskman->process_irq(0);  }
IRQ_HANDLER(irq1_handler)      { hal->taskman->process_irq(1);  }
IRQ_HANDLER(irq2_handler)      { hal->taskman->process_irq(2);  }
IRQ_HANDLER(irq3_handler)      { hal->taskman->process_irq(3);  }
IRQ_HANDLER(irq4_handler)      { hal->taskman->process_irq(4);  }
IRQ_HANDLER(irq5_handler)      { hal->taskman->process_irq(5);  }
IRQ_HANDLER(irq6_handler)      { hal->taskman->process_irq(6);  }
IRQ_HANDLER(irq7_handler)      { hal->taskman->process_irq(7);  }
IRQ_HANDLER_HIGH(irq8_handler) { hal->taskman->process_irq(8);  }
IRQ_HANDLER_HIGH(irq9_handler) { hal->taskman->process_irq(9);  }
IRQ_HANDLER_HIGH(irqa_handler) { hal->taskman->process_irq(10); }
IRQ_HANDLER_HIGH(irqb_handler) { hal->taskman->process_irq(11); }
IRQ_HANDLER_HIGH(irqc_handler) { hal->taskman->process_irq(12); }
IRQ_HANDLER_HIGH(irqd_handler) { hal->taskman->process_irq(13); }
IRQ_HANDLER_HIGH(irqe_handler) { hal->taskman->process_irq(14); }
IRQ_HANDLER_HIGH(irqf_handler) { hal->taskman->process_irq(15); }

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
set_trap(0x0f, MAKE_ISR(exceptionF), hal->sys_code);
set_trap(0x10, MAKE_ISR(exception10), hal->sys_code);
set_trap(0x11, MAKE_ISR(exception11), hal->sys_code);
set_trap(0x12, MAKE_ISR(exception12), hal->sys_code);
set_trap(0x13, MAKE_ISR(exception_unknown), hal->sys_code);
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