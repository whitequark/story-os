#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <story.h>

void* morecore(unsigned int count);
void die(unsigned int return_code);
void wait_die(unsigned int tid);
void delay(unsigned int millis);
void wait_irq(unsigned int irq);
bool map_pages(unsigned int physical_addr, unsigned int virtual_addr, unsigned int count);

#endif