#ifndef _LIB_H_INCLUDED_
#define _LIB_H_INCLUDED_

#define NULL 0

void *memset(void *b, int c, int len);
void *memcpy(void *dst, const void *src, int len);
int memcmp(const void *b1, const void *b2, int len);
void *memchr(void *b, int c, int len);
int strlen(const char *s);
char *strcpy(char *dst, const char *src);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, int len);
char *strchr(char *s, int c);

int puts(char *str);
int putxval(unsigned long value, int column);

#endif
