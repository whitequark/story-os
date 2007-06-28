#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include <msgtype.h>
#include <colors.h>

class Terminal
{
public:
enum { mtPutChar, mtPutString, mtColor };

Terminal();
void put_char(char c);
void put_string(char* s);
void color(char color);
};

#endif