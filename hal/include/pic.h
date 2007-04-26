#ifndef _PIC_H_
#define _PIC_H_

class PIC
{
public:
PIC();
void remap(char v1, char v2);
void mask(char n);
void unmask(char n);
};

#endif