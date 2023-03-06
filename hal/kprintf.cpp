//    This file is part of the Story OS
//    Copyright (C) 2007  Catherine 'whitequark'
//
//    Story OS is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    Story OS is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. 

#include <stdarg.h>
#include <kprintf.h>

void putchar(char c); 
void textcolor(char c);

void puthexd(unsigned char digit)
{
char table[]="0123456789ABCDEF";
putchar(table[digit]);
}

void putdec(unsigned int byte)
{
unsigned char b1;
int b[30];
signed int nb;
int i=0;
  
while(1)
 {
 b1=byte%10;
 b[i]=b1;
 nb=byte/10;
 if(nb<=0) break;
 i++;
 byte=nb;
 }
for(nb=i+1;nb>0;nb--)
 puthexd(b[nb-1]);
}

void puthex(unsigned char byte)
{
unsigned  char lb, rb;
lb=byte >> 4;
rb=byte & 0x0F;
puthexd(lb);
puthexd(rb);
}

void puthexi(unsigned int dword)
{
puthex((dword & 0xFF000000) >> 24);
puthex((dword & 0x00FF0000) >> 16);
puthex((dword & 0x0000FF00) >> 8 );
puthex((dword & 0x000000FF)      );
}

void puthexw(unsigned short word)
{
puthex((word & 0xFF00) >> 8 );
puthex((word & 0x00FF)      );
}

void puts(const char *s)
{
while(*s)
 putchar(*s++);
}


void vprintf(const char *fmt, va_list args)
{
char *str;
int n;
  
while (*fmt) 
 {
 switch (*fmt) 
  {
  case '%':
  fmt++;

  switch (*fmt) 
   {
   case 's':
   str = (va_arg(args, char *));
   if(str != NULL)
    puts(str);
   else
    puts("(null)");
   break;
	
   case 'c':
   putchar(va_arg(args, unsigned int));
   break;
	
   case 'i':
   case 'd':
   case 'u':
   putdec(va_arg(args, unsigned int));
   break;

   case 'x':
   n = va_arg(args, unsigned int);
   if(n <= 0xFF)
    puthex(n);
   else if(n <= 0xFFFF)
    puthexw(n);
   else
    puthexi(n);
   break;
   
   case 'X':
   puthexi(va_arg(args, unsigned int));
   break;

   case 'z':
   textcolor(va_arg(args, unsigned int));
   break;
   
   case '%':
   putchar('%');
   break;
   }
  break;
  
  default:
  putchar(*fmt);
  break;
  }
 fmt++;
 }
}

void printf(const char *fmt, ...)
{
va_list args;
va_start(args, fmt);
vprintf(fmt, args);
va_end(args);
}
