#include "defines.h"
#include "kozos.h"
#include "intr.h"
#include "interrupt.h"
#include "timer.h"
#include "lib.h"
#include "timerdrv.h"

struct timerbuf {
  struct timerbuf *next;
  kz_msgbox_id_t id; /* タイマ満了時のメッセージ送信先 */
  int msec;
};

static struct timerreg {
  struct timerbuf *timers; /* タイマ・バッファのリンクリスト */
  int index; /* 利用するタイマの番号 */
} timerreg;

/*
 * 以下は割込みハンドラであり，非同期で呼ばれるので，ライブラリ関数などを
 * 呼び出す場合には注意が必要．
 * 基本として，以下のいずれかに当てはまる関数しか呼び出してはいけない．
 * ・再入可能である．
 * ・スレッドから呼ばれることは無い関数である．
 * ・スレッドから呼ばれることがあるが，割込み禁止で呼び出している．
 * また非コンテキスト状態で呼ばれるため，システム・コールは利用してはいけない．
 * (サービス・コールを利用すること)
 */
static void timerdrv_intr(void)
{
  struct timerreg *tim = &timerreg;

  if (timer_is_expired(tim->index)) { /* タイマ割込み */
    timer_cancel(tim->index);
    kx_send(MSGBOX_ID_TIMDRIVE, TIMERDRV_CMD_EXPIRE, NULL);
  }
}

static int timerdrv_init(void)
{
  memset(&timerreg, 0, sizeof(timerreg));
  timerreg.index = TIMER_DEFAULT_DEVICE;
  return 0;
}

/* スレッドからの要求を処理する */
static int timerdrv_command(struct timerreg *tim, int cmd, char *p)
{
  struct timerbuf *tmbuf;
  struct timerbuf **tmbufp;
  struct timerreq *req;
  int t, msec;

  switch (cmd) {
  case TIMERDRV_CMD_EXPIRE: /* タイマ満了 */
    tmbuf = tim->timers;
    if (tmbuf) {
      tim->timers = tmbuf->next;
      kz_send(tmbuf->id, 0, NULL);
      kz_kmfree(tmbuf);
      if (tim->timers)
      	timer_start(tim->index, tim->timers->msec, 0);
    }
    break;

  case TIMERDRV_CMD_START: /* タイマのスタート */
    req = (struct timerreq *)p;

    tmbuf = kz_kmalloc(sizeof(*tmbuf));
    tmbuf->next = NULL;
    tmbuf->id   = req->id;
    tmbuf->msec = req->msec;

    t = 0;
    if (tim->timers) {
      t = timer_gettime(tim->index);
    }

    for (tmbufp = &tim->timers;; tmbufp = &(*tmbufp)->next) {
      if (*tmbufp == NULL) {
        *tmbufp = tmbuf;
        if (tmbufp == &tim->timers)
          timer_start(tim->index, tim->timers->msec, 0);
        break;
      }
      msec = (*tmbufp)->msec - t;
      if (msec < 0) msec = 0;
      if (tmbuf->msec < msec) {
        (*tmbufp)->msec = msec - tmbuf->msec;
        tmbuf->next = *tmbufp;
        *tmbufp = tmbuf;
        timer_start(tim->index, tim->timers->msec, 0);
        break;
      }
      t = 0;
      tmbuf->msec -= msec;
    }

    kz_kmfree(p);
    break;

  default:
    break;
  }

  return 0;
}

int timerdrv_main(int argc, char *argv[])
{
  int cmd;
  char *p;

  timerdrv_init();
  kz_setintr(SOFTVEC_TYPE_TIMINTR, timerdrv_intr); /* 割込みハンドラ設定 */

  while (1) {
    kz_recv(MSGBOX_ID_TIMDRIVE, &cmd, &p);
    timerdrv_command(&timerreg, cmd, p);
  }

  return 0;
}
