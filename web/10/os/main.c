#include "lib.h"
#include "serial.h"
#include "timer.h"
#include "interrupt.h"

#ifdef KOZOS
#include "kozos.h"
#endif

static int init()
{
  extern int bss_start;
  extern int ebss;

  /* clear BSS */
  memset(&bss_start, 0, (uint32)&ebss - (uint32)&bss_start);

  serial_initialize(0, 0);
  return 0;
}

void defhandler(int vec)
{
  thread_intr(vec);
}

void sethandler()
{
  interrupt_sethandler(VECTYPE_TRAPA0, defhandler);
  interrupt_sethandler(VECTYPE_RXI0, defhandler);
  interrupt_sethandler(VECTYPE_RXI1, defhandler);
  interrupt_sethandler(VECTYPE_RXI2, defhandler);
}

int main()
{
  init();

  DISABLE_INTR;

  puts("kozos boot succeed!\n");

  sethandler();

  ENABLE_INTR;

  kozos_start(0, NULL);

  return 0;
}
