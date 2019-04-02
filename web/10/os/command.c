#include "kozos.h"
#include "thread.h"
#include "extintr.h"
#if 0
#include "logging.h"
#endif

#include "lib.h"

uint32 command0_id;
uint32 command1_id;
uint32 command_dummy;

static int dummy_func(int n)
{
  int i, sum;
  sum = 0;
  for (i = 1; i < n; i++) {
    sum += i;
  }
  return sum;
}

static void send_use(int index)
{
  char *p;
  p = kz_kmalloc(3);
  p[0] = EXTINTR_CMD_CONSOLE;
  p[1] = '0' + index;
  p[2] = EXTINTR_CMD_CONSOLE_USE;
  kz_send(extintr_id, 3, p);
}

static void send_enable(int index)
{
  char *p;
  p = kz_kmalloc(3);
  p[0] = EXTINTR_CMD_CONSOLE;
  p[1] = '0' + index;
  p[2] = EXTINTR_CMD_CONSOLE_ENABLE;
  kz_send(extintr_id, 3, p);
}

#if 0
static void send_disable(int index)
{
  char *p;
  p = kz_kmalloc(3);
  p[0] = EXTINTR_CMD_CONSOLE;
  p[1] = '0' + index;
  p[2] = EXTINTR_CMD_CONSOLE_DISABLE;
  kz_send(extintr_id, 3, p);
}
#endif

static void send_write(int index, char *str)
{
  char *p;
  int len;
  len = strlen(str);
  p = kz_kmalloc(len + 3);
  p[0] = EXTINTR_CMD_CONSOLE;
  p[1] = '0' + index;
  p[2] = EXTINTR_CMD_CONSOLE_WRITE;
  memcpy(&p[3], str, len);
  kz_send(extintr_id, len + 3, p);
}

static int command_proc(int index)
{
  char *p;
  char buffer[32];
  int len, size, i;
#if 0
  int log;
#endif

  len = 0;
  send_write(index, "command> ");
  send_enable(index);

  while (1) {
    size = kz_recv(NULL, &p);
    send_enable(index);
    if ((size == 0)
	|| memchr(p, 0x04, size) /* Ctrl-D対応 */
	|| memchr(p, 0xff, size) /* Ctrl-C対応 */
	) break;
    memcpy(buffer + len, p, size);
    kz_kmfree(p);
    len += size;
    buffer[len] = '\0';

    p = strchr(buffer, '\n');
    if (p == NULL) continue;

    if (!strncmp(buffer, "echo", 4)) {
      send_write(index, buffer + 4);
    } else if (!strncmp(buffer, "index", 5)) {
      strcpy(buffer, "X\n");
      buffer[0] = '0' + index;
      send_write(index, buffer);
    } else if (!strncmp(buffer, "call", 4)) {
      dummy_func(10);
    } else if (!strncmp(buffer, "down", 4)) {
      int *nullp = NULL;
      *nullp = 1;
#if 0
    } else if (!strncmp(buffer, "trap", 4)) {
      kz_trap();
    } else if (!strncmp(buffer, "break", 5)) {
      kz_break();
#endif
    } else if (!strncmp(buffer, "threads", 7)) {
      kz_thread *thp;
      for (i = 0; i < THREAD_NUM; i++) {
	thp = &threads[i];
	if (!thp->id) continue;
	send_write(index, thp->name);
	send_write(index, "\n");
      }
#if 0
    } else if (!strncmp(buffer, "log", 3)) {
      logging_disable = 1;
      log = logging_cur;
      for (i = 0; i < LOGGING_BUF_SIZE; i++) {
	ub_printf("%08x%c", logging_buf[log++], ((i % 8) == 7) ? '\n' : ' ');
	if (log >= LOGGING_BUF_SIZE)
	  log = 0;
      }
      logging_disable = 0;
#endif
    } else if (!strncmp(buffer, "exit", 4)) {
      break;
    }

    len = 0;
    send_write(index, "OK\ncommand> ");
  }

  return 0;
}

int command_main(int argc, char *argv[])
{
  int index = argc;

  send_use(index);

  while (1) {
    command_proc(index);
  }

  return 0;
}
