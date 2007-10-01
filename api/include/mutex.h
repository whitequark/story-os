#ifndef _MUTEX_H_
#define _MUTEX_H_

//  xchg() from linux-2.6.17

struct __xchg_dummy { unsigned long a[100]; };
#define __xg(x) ((struct __xchg_dummy *)(x))

#define xchg(ptr,v) ((__typeof__(*(ptr)))__xchg((unsigned long)(v),(ptr),sizeof(*(ptr))))

static inline unsigned long __xchg(unsigned long x, volatile void * ptr, int size)
{
  switch (size) {
  case 1:
    __asm__ __volatile__("xchgb %b0,%1"
			 :"=q" (x)
			 :"m" (*__xg(ptr)), "0" (x)
			 :"memory");
    break;
  case 2:
    __asm__ __volatile__("xchgw %w0,%1"
			 :"=r" (x)
			 :"m" (*__xg(ptr)), "0" (x)
			 :"memory");
    break;
  case 4:
    __asm__ __volatile__("xchgl %0,%1"
			 :"=r" (x)
			 :"m" (*__xg(ptr)), "0" (x)
			 :"memory");
    break;
  }
  return x;
}

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