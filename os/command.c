#include "defines.h"
#include "kozos.h"
#include "consdrv.h"
#include "lib.h"

#if 1
#include "timer.h"
#include "intr.h"
#endif

/* コンソールドライバの使用開始をコンソールドライバに依頼する */
static void send_use(int index)
{
  char *p;
  p = kz_kmalloc(3);
  p[0] = '0';
  p[1] = CONSDRV_CMD_USE;
  p[2] = '0' + index;
  kz_send(MSGBOX_ID_CONSOUTPUT, 3, p);
}

/* コンソールへの文字列出力をコンソールドライバに依頼する */
static void send_write(char *str)
{
  char *p;
  int len;
  len = strlen(str);
  p = kz_kmalloc(len + 2);
  p[0] = '0';
  p[1] = CONSDRV_CMD_WRITE;
  memcpy(&p[2], str, len);
  kz_send(MSGBOX_ID_CONSOUTPUT, len + 2, p);
}

void timer_intr(softvec_type_t type, unsigned long sp)
{
  if (timer_is_expired(0)) {
    puts("timer expired 0.\n");
    timer_expire(0);
  }
  if (timer_is_expired(1)) {
    puts("timer expired 1.\n");
    timer_cancel(1);
  }
}

int command_main(int argc, char *argv[])
{
  char *p;
  int size;

  send_use(SERIAL_DEFAULT_DEVICE);

  while (1) {
    send_write("command> ");    /* プロンプト表示 */

    /* コンソールカラン受信文字列を受け取る*/
    kz_recv(MSGBOX_ID_CONSINPUT, &size, &p);
    p[size] = '\0';

    if (!strncmp(p, "echo", 4)) { /* echo コマンド */
      send_write(p + 4);
      send_write("\n");
    } else if (!strncmp(p, "timer", 5)) { /* タイマ起動コマンド */
      softvec_setintr(SOFTVEC_TYPE_TIMINTR, timer_intr);
      puts("start timer.\n");
      timer_start(0, 3000);
      timer_start(1, 4500);
    } else if (!strncmp(p, "cancel", 6)) {  /* タイマキャンセル */
      puts("cancel timer.\n");
      timer_cancel(0);
    } else {
      send_write("unknown.\n");
    }

    kz_kmfree(p);
  }

  return 0;
}
