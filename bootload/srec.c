#include "defines.h"
#include "lib.h"
#include "srec.h"

static char *startaddr;

int srec_init()
{
  startaddr = NULL;
  return 0;
}

static int h2d(unsigned char c)
{
  if ((c >= '0') && (c <= '9'))
    return c - '0';
  if ((c >= 'a') && (c <= 'f'))
    return c - 'a' + 0xa;
  if ((c >= 'A') && (c <= 'F'))
    return c - 'A' + 0xa;
  return -1;
}

int getval(unsigned char **pp, unsigned char *sum)
{
  int val;
  char *p;

  p = *pp;
  val  = h2d(*(p++)) << 4;
  val |= h2d(*(p++));
  *pp = p;

  *sum += val;

  return val;
}

static int decode(char *from)
{
  int i, val, type, count, addrsize, datasize, checksum;
  uint32 addr;
  unsigned char *p;
  unsigned char sum = 0;

  p = from;
  if (*(p++) != 'S')
    return 0;

  type = h2d(*(p++));
  if ((type < 0) || (type > 9))
    return 0;

  count = getval(&p, &sum);

  addr = 0;
  addrsize = 0;
  datasize = 0;

  switch (type) {
  case 0:
    addrsize = 2;
    datasize = count - addrsize - 1; /* -1はcount */
    break;
  case 3:
  case 2:
  case 1:
    addrsize = type + 1;
    datasize = count - addrsize - 1;
    break;
  case 5:
    addrsize = 2;
    break;
  case 7:
  case 8:
  case 9:
    addrsize = 11 - type;
    break;
  default:
    return -1;
  }

  for (i = 0; i < addrsize; i++) {
    addr = (addr << 8) | getval(&p, &sum);
  }

  for (i = 0; i < datasize; i++) {
    val = getval(&p, &sum);
    ((char *)addr)[i] = val;
  }

  checksum = getval(&p, &sum);

  if (sum != 0xff)
    return -1;

  if (!startaddr)
    startaddr = (char *)addr;

  return 0;
}

int srec_decode(unsigned char c)
{
  static char buf[2+2+8+64+2];
  static int i = 0;

  buf[i++] = c;
  if ((c == '\r') || (c == '\n')) {
    buf[i++] = '\0';
    i = 0;
    if (decode(buf) < 0)
      return -1;
  }

  return 0;
}

char *srec_startaddr()
{
  return startaddr;
}
