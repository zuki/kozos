#include "defines.h"
#include "kozos.h"
#include "consdrv.h"
#include "timerdrv.h"
#include "netdrv.h"
#include "icmp.h"
#include "lib.h"

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

/* タイマのカウント開始をmタイマドライバに依頼する */
static void send_start(int msec)
{
  struct timerreq *req;
  req = kz_kmalloc(sizeof(*req));
  req->id = MSGBOX_ID_CONSINPUT;
  req->msec = msec;
  kz_send(MSGBOX_ID_TIMDRIVE, TIMERDRV_CMD_START, (char *)req);
}

/* pingの開始をicmpタスクに依頼する */
static void send_icmp()
{
  struct netbuf *buf;

  buf = kz_kmalloc(sizeof(*buf));
  buf->cmd = ICMP_CMD_SEND;
  buf->option.icmp.send.number = 3;
  buf->option.icmp.send.ipaddr = 0xc0a80b07;  /* 192.167.11.7 */
  kz_send(MSGBOX_ID_ICMPPROC, 0, (char *)buf);
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
    if (p == NULL) {
      send_write("expired.\n");
      continue;
    }
    p[size] = '\0';

    if (!strncmp(p, "echo", 4)) { /* echo コマンド */
      send_write(p + 4);
      send_write("\n");
    } else if (!strncmp(p, "timer", 5)) { /* タイマ起動コマンド */
      send_write("timer start.\n");
      send_start(1000);
    } else if (!strncmp(p, "ping", 4)) { /* pingコマンド */
      send_write("ping start.\n");
      send_icmp();
    } else {
      send_write("unknown.\n");
    }

    kz_kmfree(p);
  }

  return 0;
}
