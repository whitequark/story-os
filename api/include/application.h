#ifndef _APPLICATION_H_
#define _APPLICATION_H_

namespace Procman
{
enum { mtDelay, mtDie, mtWaitForDie, mtWaitForIRQ, mtAllocPages };
};

class Application
{
private:
void delay(unsigned int millis);
void die(int return_code);
int wait_for_die(unsigned int task);
int wait_for_irq(unsigned int task);
void* alloc_pages(unsigned int count);

int run();

public:
void start();
};

#endif