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

#include <string.h>

typedef unsigned int size_t;

void *memcpy (void *dest, const void *src, size_t n)
{
asm("rep movsb"::"S"(src), "D"(dest), "c"(n));
return dest;
}

void *memcpyl (unsigned int *dest, unsigned const int *src, unsigned int n)
{
int i;
for(i = 0; i < n; i++)
 dest[i] = src[i];
return dest;
}

void *memset (void *s, int c, size_t n)
{
asm("rep stosb"::"a"(c), "D"(s), "c"(n));
return s;
}

void *memsetl(unsigned int *s, unsigned int c, unsigned int n)
{
int i;
for(i = 0; i < n; i++)
 s[i] = c;
return s;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
if (n != 0)
 {
 const char *p1 = (char*)s1, *p2 = (char*)s2;
 do
  {
  if (*p1++ != *p2++)
  return (*--p1 - *--p2);
  } 
 while (--n != 0);
 }
return 0;
}

unsigned int mem_to_pages(unsigned int mem)
{
return ((mem/0x1000) + 1);
}
