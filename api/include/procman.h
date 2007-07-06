#ifndef _PROCMAN_H_
#define _PROCMAN_H_

class Procman
{
public:
enum { mtDelay, mtDie, mtWaitForDie, mtWaitForIRQ, mtAllocPages, mtCreateThread };
void delay(unsigned int millis);
void die(int return_code);
int wait_for_die(unsigned int task);
void wait_for_irq(unsigned int task);
void* alloc_pages(unsigned int count);
unsigned int create_thread(void* entry_point);

void printf(char* fmt, ...);
};

#endif