#ifndef _STRING_H_
#define _STRING_H_

#include <stdlib.h>

void *memcpy (void *dest, const void *src, size_t n);
void *memcpyl (unsigned int *dest, unsigned const int *src, unsigned int n);
void *memset (void *s, int c, size_t n);
void *memsetl(unsigned *s, unsigned int c, unsigned int n);
int memcmp(const void *s1, const void *s2, size_t n);

size_t strlen(const char* str);
char* strcpy(char* dest, const char* src);
int strcmp(const char* string1, const char* string2 );
int strncmp(const char* string1, const char* string2, int n );
char* strncpy(char* dest, const char* src, int n);
char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, int n);
int atoi(const char *s);
int is_digit(char c);
char* strdup(char* src);

#endif
