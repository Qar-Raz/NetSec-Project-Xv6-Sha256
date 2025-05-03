#ifndef _KERNEL_STRING_H_ // Include guard to prevent multiple inclusions
#define _KERNEL_STRING_H_

#include "types.h" // Include types.h for uint, uchar definitions

// Function Prototypes from string.c

void*   memset(void *dst, int c, uint n);
int     memcmp(const void *v1, const void *v2, uint n);
void*   memmove(void *dst, const void *src, uint n);
void*   memcpy(void *dst, const void *src, uint n);
int     strncmp(const char *p, const char *q, uint n);
char*   strncpy(char *s, const char *t, int n);
char*   safestrcpy(char *s, const char *t, int n);
int     strlen(const char *s);

#endif // _KERNEL_STRING_H_
