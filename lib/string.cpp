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

size_t strlen(const char* str)
{
const char* str1 = str;
while(*str1++);
return str1 - str - 1;
}

char* strcpy(char* dest, const char* src)
{
memcpy(dest, src, strlen(src) + 1);
return dest;
}

char* strncpy(char* dest, const char* src, int n)
{
int x = n < strlen(src) ? n : strlen(src);
memcpy(dest, src, x);
return dest;
}

char *strncat(char *dest, const char *src, int n)
{
const char *start;
char *ret;
  
ret = dest;
while(*dest != 0)
 dest++;
start = src;

while(*src != 0 && (src - start) < n)
 {
 *dest = *src;
 dest++;
 src++;
 }

*dest = 0;
return ret;
}

char *strcat(char *dest, const char *src)
{
return strncat (dest, src, strlen(src));
}

int strcmp(const char *s1, const char *s2)
{
return strncmp(s1, s2, strlen(s1) > strlen(s2) ? strlen(s1) : strlen(s2));
}

int strncmp(const char *s1, const char *s2, int n)
{
if(n == 0)
 return 0;
do 
 {
 if(*s1 != *s2++)
  return *(unsigned const char *)s1 - *(unsigned const char *)--s2;
 if (*s1++ == 0)
  break;
 } 
while (--n != 0);
return 0;
}

int tens_power(int n)
{
int i, value = 1;
for(i = 1; i <= n; i++)
 value *= 10;
return value;
}

int atoi(const char *s)
{
char digit[0x10];
signed int counter=0;
signed int i;
int value = 0;
  
while(is_digit(*s))
 {
 digit[counter] = *s - '0';
 s++;
 counter++;
 }

for(i = counter - 1; i >= 0; i--)
 value += digit[i] * tens_power(((counter-1)-i));

return value;
}

int is_digit(char c)
{
if(c >= '0' && c <= '9')
 return 1;
else
 return 0;
}

char* strdup(char* src)
{
char* r = new char[strlen(src)+1];
r[strlen(src)] = 0;
strcpy(r, src);
return r;
}