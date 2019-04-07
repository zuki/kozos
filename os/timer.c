#include "defines.h"
#include "timer.h"

#define TIMER_TMR_NUM 2

#define H8_3069F_TMR01  ((volatile struct h8_3069f_tmr *)0xffff80)
#define H8_3069F_TMR23  ((volatile struct h8_3069f_tmr *)0xffff90)

struct h8_3069f_tmr {
  volatile uint8 tcr0;
  volatile uint8 tcr1;
  volatile uint8 tcsr0;
  volatile uint8 tcsr1;
  volatile uint8 tcora0;
  volatile uint8 tcora1;
  volatile uint8 tcorb0;
  volatile uint8 tcorb1;
  volatile uint16 tcnt;
};

#define H8_3069F_TMR_TCR_DISCLK       (0<<0)
#define H8_3069F_TMR_TCR_CLK8         (1<<0)
#define H8_3069F_TMR_TCR_CLK64        (2<<0)
#define H8_3069F_TMR_TCR_CLK8192      (3<<0)
#define H8_3069F_TMR_TCR_OVF          (4<<0)
#define H8_3069F_TMR_TCR_CMFA         (4<<0)
#define H8_3069F_TMR_TCR_CLKUP        (5<<0)
#define H8_3069F_TMR_TCR_CLKDOWN      (6<<0)
#define H8_3069F_TMR_TCR_CLKBOTH      (7<<0)

#define H8_3069F_TMR_TCR_CCLR_DISCLR  (0<<3)
#define H8_3069F_TMR_TCR_CCLR_CLRCMFA (1<<3)
#define H8_3069F_TMR_TCR_CCLR_CLRCMFB (2<<3)
#define H8_3069F_TMR_TCR_CCLR_DISINPB (3<<3)

#define H8_3069F_TMR_TCR_OVIE         (1<<5)
#define H8_3069F_TMR_TCR_CMIEA        (1<<6)
#define H8_3069F_TMR_TCR_CMIEB        (1<<7)

#define H8_3069F_TMR_TCSR_OS_NOACT    (0<<0)
#define H8_3069F_TMR_TCSR_OIS_NOACT   (0<<2)
#define H8_3069F_TMR_TCSR_ADTE        (1<<4)
#define H8_3069F_TMR_TCSR_ICE         (1<<4)
#define H8_3069F_TMR_TCSR_OVF         (1<<5)
#define H8_3069F_TMR_TCSR_CMFA        (1<<6)
#define H8_3069F_TMR_TCSR_CMFB        (1<<7)

static struct {
  volatile struct h8_3069f_tmr *tmr;
} regs[TIMER_TMR_NUM] = {
  { H8_3069F_TMR01 },
  { H8_3069F_TMR23 },
};

/* タイマ開始 */
int timer_start(int index, int msec, int flags)
{
  volatile struct h8_3069f_tmr *tmr = regs[index].tmr;
  int   count;
  uint8 tcr;

  tcr = H8_3069F_TMR_TCR_OVF;
  if (flags & TIMER_START_FLAG_CYCLE)
    tcr |= H8_3069F_TMR_TCR_CCLR_CLRCMFA;
  else
    tcr |= H8_3069F_TMR_TCR_CCLR_DISCLR;

  tmr->tcr0 = tcr;
  tmr->tcr1 = H8_3069F_TMR_TCR_CLK8192 | H8_3069F_TMR_TCR_CCLR_DISCLR;

  tmr->tcsr0 = 0;
  tmr->tcsr1 = 0;

  count = msec / 105;   /* 20MHz: (msec * 20,000,000 / 8192 / 256 / 1000) */

  tmr->tcnt = 0;
  tmr->tcora0 = count;
  tmr->tcr0 |= H8_3069F_TMR_TCR_CMIEA;    /* 割込み有効化 */

  return 0;
}

/* タイマ満了したか？ */
int timer_is_expired(int index)
{
  volatile struct h8_3069f_tmr *tmr = regs[index].tmr;
  return (tmr->tcsr0 & H8_3069F_TMR_TCSR_CMFA) ? 1 : 0;
}

/* タイマ満了処理 */
int timer_expire(int index)
{
  volatile struct h8_3069f_tmr *tmr = regs[index].tmr;

  tmr->tcsr0 &= ~H8_3069F_TMR_TCSR_CMFA;

  return 0;
}

/* タイマキャンセル */
int timer_cancel(int index)
{
  volatile struct h8_3069f_tmr *tmr = regs[index].tmr;

  timer_expire(index);

  tmr->tcr0 = 0;
  tmr->tcr1 = 0;

  tmr->tcr0 &= ~H8_3069F_TMR_TCR_CMIEA;   /* 割込み無効化 */

  return 0;
}

/* タイマ動作中か？ */
int timer_is_running(int index)
{
    volatile struct h8_3069f_tmr *tmr = regs[index].tmr;
    return (tmr->tcr0 & H8_3069F_TMR_TCR_CMIEA) ? 1 : 0;
}

/* タイマの現在値 */
int timer_gettime(int index)
{
  volatile struct h8_3069f_tmr *tmr = regs[index].tmr;
  volatile int count;
  int msec;

  /*
   * 周期タイマの場合は動作中かどうかのチェックの直後にタイマ満了すると
   * カウンタがセロに初期化されてしまうので、前もって値を取得しておく。
   */
  count = tmr->tcnt;
  msec = count * 2 / 5; /* 20MHz: (count * 8192 * 1000 / 200000000) */

  return timer_is_running(index) ? msec : -1;
}
