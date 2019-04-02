#include "serial.h"
#include "lib.h"

void *memset(void *b, int c, int len)
{
  char *p;
  for (p = b; len > 0; len--)
    *(p++) = c;
  return b;
}

void *memcpy(void *dst, const void *src, int len)
{
  char *d = dst;
  const char *s = src;
  for (; len > 0; len--)
    *(d++) = *(s++);
  return dst;
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

void *memchr(void *b, int c, int len)
{
  char *p = b;
  for (; len > 0; len--) {
    if (*p == c)
      return p;
    p++;
  }
  return NULL;
}

int strlen(const char *s)
{
  int len;
  for (len = 0; *s; s++, len++)
    ;
  return len;
}

char *strcpy(char *dst, const char *src)
{
  char *d = dst;
  for (;; dst++, src++) {
    *dst = *src;
    if (!*src) break;
  }
  return d;
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
  for (; len > 0; len--) {
    if (*(s1++) != *(s2++)) return 1;
  }
  return 0;
}

char *strchr(char *s, int c)
{
  for (; *s; s++) {
    if (*s == c)
      return s;
  }
  return NULL;
}

int puts(char *str)
{

  while (*str)
    serial_putc(1, *(str++));
  return 0;
}

#if 0
int putval(unsigned long value, int column)
{
  char buf[12];
  char *p;

  p = buf + sizeof(buf) - 1;
  *(p--) = '\0';

  if (!value && !column)
    column++;

  while (value) {
    *(p--) = (value % 10) + '0';
    value = value / 10;
    if (column) column--;
  }

  puts(p + 1);

  return 0;
}
#endif

#if 1
int putxval(unsigned long value, int column)
{
  char buf[9];
  char *p;

  p = buf + sizeof(buf) - 1;
  *(p--) = '\0';

  if (!value && !column)
    column++;

  while (value || column) {
    *(p--) = "0123456789abcdef"[value & 0xf];
    value >>= 4;
    if (column) column--;
  }

  puts(p + 1);

  return 0;
}
#endif

#if 0
static int gets(char *buf)
{
  int i = 0, c = -1;
  while (c) {
    c = serial_getc(1);
    serial_putc(1, c); /* echo back */
    if (c == '\n')
      c = '\0';
    buf[i++] = c;
  }
  return i - 1;
}
#endif

#if 0
static int dump(char *buf, int size)
{
  int i;

  if (size < 0) {
    puts("no data.\n");
    return -1;
  }
  for (i = 0; i < size; i++) {
    putxval(buf[i], 2);
    if ((i & 0xf) == 15) {
      puts("\n");
    } else {
      if ((i & 0xf) == 7) puts(" ");
      puts(" ");
    }
  }
  puts("\n");

  return 0;
}
#endif
