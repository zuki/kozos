#include "kozos.h"
#include "thread.h"
#include "extintr.h"
#include "serial.h"

#include "lib.h"

/* #define USE_MESSAGE */

#define BUFFER_SIZE 16

uint32 extintr_id;

static int intrpri_current = EXTINTR_INTERRUPTS_NUM;

static struct consreg {
  struct interrupts *interrupt;
} consreg[EXTINTR_CONSOLE_NUM];

static struct interrupts {

  uint32 id;

  enum {
    INTERRUPTS_TYPE_UNKNOWN = 0,
    INTERRUPTS_TYPE_CONSOLE,
  } type;

  union {
    void *reg;
    struct consreg *cons;
  } regs;

  int (*mask)(struct interrupts *intp);
  int (*unmask)(struct interrupts *intp);
  int (*checkintr)(struct interrupts *intp);
  int (*intr)(struct interrupts *intp);
  int (*command)(struct interrupts *intp, uint32 id, int size, char *command);

} interrupts[EXTINTR_INTERRUPTS_NUM];

typedef int (*initfunc)(struct interrupts *intp);

static void extintr_handler(int signo);

static int consreg_init()
{
  int i;
  struct consreg *crp;

  for (i = 0; i < EXTINTR_CONSOLE_NUM; i++) {
    crp = &consreg[i];
    memset(crp, 0, sizeof(*crp));
  }

  return 0;
}

static int regs_init()
{
  consreg_init();
  return 0;
}

static int extregs_init()
{
  regs_init();

#ifdef USE_MESSAGE
  kz_setsig(SIGHUP);
#else
  kz_sethandler(SIGHUP, extintr_handler);
#endif

  return 0;
}

static int interrupts_init()
{
  memset(interrupts, 0, sizeof(interrupts));
  return 0;
}

static int cons_mask(struct interrupts *intp)
{
  serial_intr_disable(intp - interrupts);
  return 0;
}

static int cons_unmask(struct interrupts *intp)
{
  serial_intr_enable(intp - interrupts);
  return 0;
}

static int cons_checkintr(struct interrupts *intp)
{
  return serial_tstc(intp - interrupts);
}

static int cons_intr(struct interrupts *intp)
{
  int size;
  char *buffer;

#ifdef USE_MESSAGE
  buffer = kz_kmalloc(BUFFER_SIZE);
#else
  buffer = kx_kmalloc(BUFFER_SIZE);
#endif
  size = 1;

  buffer[0] = serial_getc(intp - interrupts);
  if (buffer[0] == '\r')
    buffer[0] = '\n';
  serial_putc(intp - interrupts, buffer[0]); /* echo back */

  if ((size >= 0) && intp->id)
#ifdef USE_MESSAGE
    kz_send(intp->id, size, buffer);
#else
    kx_send(intp->id, size, buffer);
#endif
  else
#ifdef USE_MESSAGE
    kz_kmfree(buffer);
#else
    kx_kmfree(buffer);
#endif

  return 0;
}

static int cons_command(struct interrupts *intp, uint32 id, int size, char *command)
{
  switch (command[0]) {
  case EXTINTR_CMD_CONSOLE_USE: /* コンソールの利用 */
    intp->id = id;
    break;

  case EXTINTR_CMD_CONSOLE_ENABLE:
    serial_intr_enable(intp - interrupts);
    break;

  case EXTINTR_CMD_CONSOLE_DISABLE:
    serial_intr_disable(intp - interrupts);
    break;

  case EXTINTR_CMD_CONSOLE_WRITE:
    {
      int i;
      for (i = 1; i < size; i++)
	serial_putc(intp - interrupts, command[i]);
    }
    break;

  default:
    break;
  }

  return 0;
}

static int cons_init(struct interrupts *intp)
{
  intp->type      = INTERRUPTS_TYPE_CONSOLE;
  intp->mask      = cons_mask;
  intp->unmask    = cons_unmask;
  intp->checkintr = cons_checkintr;
  intp->intr      = cons_intr;
  intp->command   = cons_command;
  intp->regs.cons->interrupt = intp;

  serial_init(intp - interrupts);

  return 0;
}

static int extintr_intr_regist(struct interrupts *intp, void *regs, initfunc init)
{
  intp->regs.reg = regs;
  init(intp);
  return 0;
}

static void extintr_handler(int signo)
{
  int i;
  struct interrupts *intp;
  for (i = 0; i < intrpri_current; i++) {
    intp = &interrupts[i];
    if (intp->type == INTERRUPTS_TYPE_UNKNOWN)
      continue;
    if (intp->checkintr(intp)) {
      intp->intr(intp);
    }
  }
  return;
}

static int extintr_mainloop()
{
  int size;
  uint32 id;
  char *p;
  struct interrupts *intp;

  while (1) {
    size = kz_recv(&id, &p);

    if (!id) {
      /* 子プロセスからの割り込み通知 */
      extintr_handler(SIGHUP);
    } else {
      intp = NULL;
      switch (p[0]) {
      case EXTINTR_CMD_CONSOLE:
	intp = consreg[p[1] - '0'].interrupt;
	break;
      default:
	break;
      }
      if (intp) {
	intp->command(intp, id, size - 2, p + 2);
      }

      kz_kmfree(p);
    }
  }

  return 0;
}

int extintr_main(int argc, char *argv[])
{
  extregs_init();

  interrupts_init();

  extintr_intr_regist(&interrupts[1], &consreg[1],  cons_init);

  extintr_mainloop();

  return 0;
}
