#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include <msgtype.h>

class Terminal
{
public:
enum { mtPutChar, mtPutString };

Terminal();
void put_char(char c);
void put_string(char* s);
};

#endif