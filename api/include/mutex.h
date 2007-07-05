#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <system.h>

// idea stolen from SadKo XSystem

class Mutex
{
private:
volatile unsigned int locker;

public:
Mutex();
bool lock();
bool unlock();
bool try_lock();
bool is_locked();
};

#endif