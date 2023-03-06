#ifndef _GDT_H_
#define _GDT_H_

#include <story.h>

class GDTDescriptor
{
public:
//provides to GDT pointer to 8 bytes of descriptor
virtual char* get_bytes() = 0;
};

class NullDescriptor: public GDTDescriptor
{
public:
char* get_bytes();
};

class SegmentDescriptor: public GDTDescriptor
{
private:
char bytes[8];

SegmentDescriptor();

public:
char* get_bytes();
SegmentDescriptor(unsigned int base, unsigned int limit, bool execute, bool readwrite, char dpl = 0, bool op32bit = 1, bool conforming = 0);
};

class TSSDescriptor: public GDTDescriptor
{
private:
char bytes[8];

TSSDescriptor();

public:
char* get_bytes();
TSSDescriptor(unsigned int base);
};

class GDT
{
private:
char gdt[1024 * 8]; //there are enough descriptors!!
int count;
bool installed;

public:
GDT();
int add_descriptor(GDTDescriptor* desc);
void show();
void install();
unsigned short make_segment(unsigned short number, char dpl);
void modify_descriptor(GDTDescriptor* desc, unsigned short number);
};

#endif
