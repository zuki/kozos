#include "lib.h"
#include "uudecode.h"

static int c2b(int c)
{
  return (c == '`') ? 0 : (c - 0x20);
}

static char *decode(char *to, char *from)
{
  int len;
  unsigned char c[4], b[3];

  if (!strncmp(from, "begin ", 6) || !strncmp(from, "end\n", 4))
    return to;

  len = c2b(*from);

  from++;
  while (len > 0) {
    c[0] = c2b(*(from++));
    c[1] = c2b(*(from++));
    c[2] = c2b(*(from++));
    c[3] = c2b(*(from++));
    b[0] = (c[0] << 2) | ((c[1] >> 4) & 0x03);
    b[1] = (c[1] << 4) | ((c[2] >> 2) & 0x0f);
    b[2] = (c[2] << 6) |   c[3];
    if (len > 0) { *(to++) = b[0]; len--; }
    if (len > 0) { *(to++) = b[1]; len--; }
    if (len > 0) { *(to++) = b[2]; len--; }
  }

  return to;
}

char *uu_decode(char *to, unsigned char c)
{
  static char buf[(63/3*4) + 3];
  static int i = 0;

  buf[i++] = c;
  if (c == '\n') {
    to = decode(to, buf);
    i = 0;
  }

  return to;
}
