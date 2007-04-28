#ifndef _CLOCK_H_
#define _CLOCK_H_

class Clock
{
private:
unsigned int ticks;
unsigned int hz;

public:
Clock();
void tick();
unsigned int ms_to_ticks(unsigned int ms);
};

#endif