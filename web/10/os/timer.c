#include "types.h"
#include "interrupt.h"
#include "serial.h"
#include "timer.h"

#define H8_3069F_ITU ((volatile struct h8_3069f_itu *)0xffff60)

struct h8_3069f_itu {
  volatile uint8 tstr;
  volatile uint8 tsnc;
  volatile uint8 tmdr;
  volatile uint8 tolr;
  volatile uint8 tisra;
  volatile uint8 tisrb;
  volatile uint8 tisrc;
};

#define H8_3069F_ITU_CH0 ((volatile struct h8_3069f_itu_ch *)0xffff68)
#define H8_3069F_ITU_CH1 ((volatile struct h8_3069f_itu_ch *)0xffff70)
#define H8_3069F_ITU_CH2 ((volatile struct h8_3069f_itu_ch *)0xffff78)

struct h8_3069f_itu_ch {
  volatile uint8 tcr;
  volatile uint8 tior;
  volatile uint8 tcnth;
  volatile uint8 tcntl;
  volatile uint8 grah;
  volatile uint8 gral;
  volatile uint8 grbh;
  volatile uint8 grbl;
};

#define H8_3069F_ITU_TSTR_STR0 (1<<0)
#define H8_3069F_ITU_TSTR_STR1 (1<<1)
#define H8_3069F_ITU_TSTR_STR2 (1<<2)

#define H8_3069F_ITU_TSNC_SYNC0 (1<<0)
#define H8_3069F_ITU_TSNC_SYNC1 (1<<1)
#define H8_3069F_ITU_TSNC_SYNC2 (1<<2)

#define H8_3069F_ITU_TMDR_PWM0 (1<<0)
#define H8_3069F_ITU_TMDR_PWM1 (1<<1)
#define H8_3069F_ITU_TMDR_PWM2 (1<<2)
#define H8_3069F_ITU_TMDR_FDIR (1<<5)
#define H8_3069F_ITU_TMDR_MDF  (1<<6)

#define H8_3069F_ITU_TOLR_TOA0 (1<<0)
#define H8_3069F_ITU_TOLR_TOB0 (1<<1)
#define H8_3069F_ITU_TOLR_TOA1 (1<<2)
#define H8_3069F_ITU_TOLR_TOB1 (1<<3)
#define H8_3069F_ITU_TOLR_TOA2 (1<<4)
#define H8_3069F_ITU_TOLR_TOB2 (1<<5)

#define H8_3069F_ITU_TISRA_IMFX0  (1<<0)
#define H8_3069F_ITU_TISRA_IMFX1  (1<<1)
#define H8_3069F_ITU_TISRA_IMFX2  (1<<2)
#define H8_3069F_ITU_TISRA_IMIEX0 (1<<4)
#define H8_3069F_ITU_TISRA_IMIEX1 (1<<5)
#define H8_3069F_ITU_TISRA_IMIEX2 (1<<6)

#define H8_3069F_ITU_CH_TCR_TPSC_PER1 0
#define H8_3069F_ITU_CH_TCR_TPSC_PER2 1
#define H8_3069F_ITU_CH_TCR_TPSC_PER4 2
#define H8_3069F_ITU_CH_TCR_TPSC_PER8 3
#define H8_3069F_ITU_CH_TCR_CKEG_UP    0
#define H8_3069F_ITU_CH_TCR_CKEG_DOWN (1<<3)
#define H8_3069F_ITU_CH_TCR_CKEG_EACH (2<<3)
#define H8_3069F_ITU_CH_TCR_CCLR_GRA  (1<<5)
#define H8_3069F_ITU_CH_TCR_CCLR_GRB  (2<<5)
#define H8_3069F_ITU_CH_TCR_CCLR_SYNC (3<<5)

#define H8_3069F_ITU_CH_TIOR_IOA_NOUT   0
#define H8_3069F_ITU_CH_TIOR_IOA_OUT0   1
#define H8_3069F_ITU_CH_TIOR_IOA_OUT1   2
#define H8_3069F_ITU_CH_TIOR_IOA_TOGGLE 3
#define H8_3069F_ITU_CH_TIOR_IOB_NOUT   0
#define H8_3069F_ITU_CH_TIOR_IOB_OUT0   (1<<4)
#define H8_3069F_ITU_CH_TIOR_IOB_OUT1   (2<<4)
#define H8_3069F_ITU_CH_TIOR_IOB_TOGGLE (3<<4)

void timer_interrupt()
{
  volatile struct h8_3069f_itu *itu = H8_3069F_ITU;
  int status;
  static int count = 0;

  /*いったん読んでからIMFA0にゼロを書き込むことで割り込みフラグを落とす */
  status = itu->tisra;
  if (status & H8_3069F_ITU_TISRA_IMFX0) {
    itu->tisra &= ~H8_3069F_ITU_TISRA_IMFX0;
    count++;
    if (count >= 50) { /* 1/50秒周期なので，１秒おきに文字出力する */
      serial_putc(1, 'I');
      count = 0;
    }
  }

  return;
}

int timer_init()
{
  volatile struct h8_3069f_itu *itu = H8_3069F_ITU;
  volatile struct h8_3069f_itu_ch *ch = H8_3069F_ITU_CH0;

  interrupt_sethandler(VECTYPE_IMIA0, timer_interrupt);

  ch->tcr =
    H8_3069F_ITU_CH_TCR_TPSC_PER8 |
    H8_3069F_ITU_CH_TCR_CKEG_UP |
    H8_3069F_ITU_CH_TCR_CCLR_GRA;

  ch->tior = 0;
  ch->tcntl = 0;
  ch->tcnth = 0;

  /* 1/50秒周期でタイマがかかるように設定(20MHz でプリスケーラが1/8) */
  ch->gral = 50000 & 0xff;
  ch->grah = (50000 >> 8) & 0xff;

  itu->tsnc = 0;
  itu->tmdr = 0;
  itu->tisra = H8_3069F_ITU_TISRA_IMIEX0;
  itu->tstr = H8_3069F_ITU_TSTR_STR0;

  return 0;
}
