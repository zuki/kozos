#include "lib.h"
#include "serial.h"
#include "xmodem.h"
#include "srec.h"
#include "elf.h"
#include "interrupt.h"

static int init()
{
  extern int erodata, data_start, edata, bss_start, ebss;

  /*
   * データ領域とBSSを初期化する．この処理以降でないと，
   * グローバル変数が初期化されていないので注意．
   */
  memcpy(&data_start, &erodata, (char *)&edata - (char *)&data_start);
  memset(&bss_start, 0, (char *)&ebss - (char *)&bss_start);

  /* ソフトウエア・割り込みベクタをゼロクリアする */
  interrupt_init();

  serial_init();

  return 0;
}

static int puts(char *str)
{
  while (*str)
    serial_putc(*(str++));
  return 0;
}

#if 0
static int putval(unsigned long value, int column)
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

static int putxval(unsigned long value, int column)
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

static int gets(char *buf)
{
  int i = 0, c = -1;
  while (c) {
    c = serial_getc();
    serial_putc(c); /* echo back */
    if (c == '\n')
      c = '\0';
    buf[i++] = c;
  }
  return i - 1;
}

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

int main()
{
  char buf[80];
  int size = -1;
  char *entry_point;
  void (*f)();
  unsigned char *loadbuf;
  extern int buffer_start;

  init();

  puts("kzboot (kozos boot loader) started.\n");

  while (1) {
    puts("kzboot> ");
    gets(buf);

    if (!strcmp(buf, "load") || !strcmp(buf, "loadelf")) {
      if (!strcmp(buf, "load"))
	loadbuf = NULL;
      else
	loadbuf = (char *)(&buffer_start);
      size = xmodem_recv(loadbuf);
      if (size < 0) {
	puts("XMODEM receive error!\n");
      } else {
	puts("XMODEM receive succeeded.\n");
      }
    } else if (!strcmp(buf, "dump")) {
      puts("size: ");
      putxval(size, 0);
      puts("\n");
      dump(loadbuf, size);
    } else if (!strcmp(buf, "run") || !strcmp(buf, "runelf")) {
      if (!strcmp(buf, "run"))
	entry_point = srec_startaddr();
      else
	entry_point = elf_load(loadbuf);
      if (!entry_point) {
	puts("run error!\n");
      } else {
	puts("starting from entry point: ");
	putxval((unsigned long)entry_point, 0);
	puts("\n");
	f = (void (*)())entry_point;
	f();
      }
    } else {
      puts("unknown command.\n");
    }

  }

  return 0;
}
