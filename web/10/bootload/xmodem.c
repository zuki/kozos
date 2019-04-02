#include "lib.h"
#include "serial.h"
#include "srec.h"
#include "uudecode.h"
#include "xmodem.h"

/* #define USE_UUENCODE */

#define XMODEM_SOH 0x01
#define XMODEM_STX 0x02
#define XMODEM_EOT 0x04
#define XMODEM_ACK 0x06
#define XMODEM_NAK 0x15
#define XMODEM_CAN 0x18
#define XMODEM_EOF 0x1a /* Ctrl-Z */

#define XMODEM_BLOCK_SIZE 128

static int xmodem_wait()
{
  int cnt0 = 0, cnt1 = 0;

  while (!serial_tstc()) {
    if (++cnt0 >= 200) {
      cnt0 = 0;
      if (++cnt1 >= 1000) {
	cnt1 = 0;
	serial_putc(XMODEM_NAK);
      }
    }
  }

  return 0;
}

static char *xmodem_read_block(char *buf)
{
  unsigned char c, block_num, check_sum;
  int i;

  block_num  = serial_getb();
  block_num ^= serial_getb();

  if (block_num != 0xff)
    return NULL;

  check_sum = 0;
  for (i = 0; i < XMODEM_BLOCK_SIZE; i++) {
    c = serial_getb();
    if (!buf) {
      if (srec_decode(c) < 0)
	return NULL;
    } else {
#ifdef USE_UUENCODE
      buf = uu_decode(buf, c);
#else
      *(buf++) = c;
#endif
    }
    check_sum += c;
  }

  check_sum ^= serial_getb();
  if (check_sum)
    return NULL;

  return buf;
}

int xmodem_recv(char *buf)
{
  int receiving = 0, size = 0;
  unsigned char c;
  unsigned char *p;

  srec_init();

  while (1) {
    if (!receiving)
      xmodem_wait();

    c = serial_getb();

    if (c == XMODEM_EOT) {
      serial_putc(XMODEM_ACK);
      break;
    } else if (c == XMODEM_SOH) {
      receiving++;
      p = xmodem_read_block(buf);
      if (buf && !p) {
	serial_putc(XMODEM_NAK);
      } else {
	if (p)
	  buf = p;
	size += XMODEM_BLOCK_SIZE;
	serial_putc(XMODEM_ACK);
      }
    } else {
      if (receiving)
	return -1;
    }
  }

  return size;
}
