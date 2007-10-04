#ifndef _STDLIB_H_
#define _STDLIB_H_

typedef unsigned int size_t;

void *malloc (size_t size);
void *calloc (size_t num, size_t size);
void *realloc (void *ptr, size_t size);
void free (void *ptr);

inline unsigned int bytes_to_pages(unsigned int bytes)
 { return (bytes % 0x1000 == 0) ? (bytes / 0x1000) : (bytes / 0x1000 + 1); }

#endif
