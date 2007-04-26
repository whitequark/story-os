#ifndef _STDIO_H_
#define _STDIO_H_

#include <stdarg.h>
#include <story.h>

void printf(const char *fmt, ...);
void puthexi(unsigned int dword);
void puthex(unsigned char byte);
void puts(const char *s);
void vprintf(const char *fmt, va_list args);

#endif
