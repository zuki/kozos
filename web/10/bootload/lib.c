#include "lib.h"

void *memcpy(void *dst, const void *src, int len)
{
  char *d = dst;
  const char *s = src;
  for (; len > 0; len--)
    *(d++) = *(s++);
  return dst;
}

void *memset(void *b, int c, int len)
{
  char *d = b;
  for (; len > 0; len--)
    *(d++) = c;
  return b;
}

int memcmp(const void *b1, const void *b2, int len)
{
  const char *p1 = b1, *p2 = b2;
  for (; len > 0; len--) {
    if (*p1 != *p2)
      return (*p1 > *p2) ? 1 : -1;
    p1++;
    p2++;
  }
  return 0;
}

int strlen(const char *s)
{
  int len;
  for (len = 0; *s; s++)
    len++;
  return len;
}

char *strcpy(char *dst, const char *src)
{
  char *d = dst;
  while (*src)
    *(d++) = *(src++);
  *d = '\0';
  return dst;
}

int strcmp(const char *s1, const char *s2)
{
  while (*s1 || *s2) {
    if (*s1 != *s2)
      return (*s1 > *s2) ? 1 : -1;
    s1++;
    s2++;
  }
  return 0;
}

int strncmp(const char *s1, const char *s2, int len)
{
  while ((*s1 || *s2) && (len > 0)) {
    if (*s1 != *s2)
      return (*s1 > *s2) ? 1 : -1;
    s1++;
    s2++;
    len--;
  }
  return 0;
}
