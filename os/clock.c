#include "defines.h"
#include "kozos.h"
#include "consdrv.h"
#include "timerdrv.h"
#include "lib.h"

/* タイマのカウント開始をタイマ・ドライバに依頼する */
static void send_start(int msec)
{
  struct timerreq *req;
  req = kz_kmalloc(sizeof(*req));
  req->id = MSGBOX_ID_TIMEXPIRE;
  req->msec = msec;
  kz_send(MSGBOX_ID_TIMDRIVE, TIMERDRV_CMD_START, (char *)req);
}

/* コンソールへの文字列出力をコンソール・ドライバに依頼する */
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

int clock_main(int argc, char *argv[])
{
  send_start(3000);

  while (1) {
    kz_recv(MSGBOX_ID_TIMEXPIRE, NULL, NULL);
    send_write("ready.\n");
    send_start(3000);
  }

  return 0;
}
