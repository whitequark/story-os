#ifndef _SCREEN_DRIVER_H_
#define _SCREEN_DRIVER_H_

#include <colors.h>

//abstract screen driver
class TerminalDriver
{
public:
virtual void put_char(char ch, int x = -1, int y = -1) = 0;
virtual void set_color(unsigned char color) = 0;
virtual void clear() = 0;
};

class KernelTerminalDriver: public TerminalDriver
{
private:
int cursorx;
int cursory;
void update_cursor();
unsigned short *lfb;
unsigned char color;

public:
KernelTerminalDriver();

void put_char(char ch, int x, int y);
void set_color(unsigned char color);
void clear();
};

#endif