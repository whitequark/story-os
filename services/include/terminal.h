#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include <msgtype.h>
#include <colors.h>

class Terminal
{
public:
enum { mtPutChar, mtPutString, mtColor };

Terminal();
void put_string(const char* s);
void color(char color);
};

#endif