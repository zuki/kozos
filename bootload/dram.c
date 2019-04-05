#include "defines.h"
#include "lib.h"
#include "dram.h"

#define H8_3069F_ABWCR    ((volatile uint8 *)0xfee020)
#define H8_3069F_ASTCR    ((volatile uint8 *)0xfee021)
#define H8_3069F_RTCOR    ((volatile uint8 *)0xfee02a)
#define H8_3069F_RTMCSR   ((volatile uint8 *)0xfee028)
#define H8_3069F_DRCRB    ((volatile uint8 *)0xfee027)
#define H8_3069F_DRCRA    ((volatile uint8 *)0xfee026)

#define H8_3069F_P1DDR    ((volatile uint8 *)0xfee000)
#define H8_3069F_P2DDR    ((volatile uint8 *)0xfee001)
#define H8_3069F_P8DDR    ((volatile uint8 *)0xfee007)
#define H8_3069F_PBDDR    ((volatile uint8 *)0xfee00a)

#define H8_3069F_WCRH     ((volatile uint8 *)0xfee022)
#define H8_3069F_WCRL     ((volatile uint8 *)0xfee023)


typedef struct {
  union {
    volatile uint8    val8[4];
    volatile uint16   val16[2];
    volatile uint32   val32[1];
  } u;
} val_t;

int dram_init(void)
{
  /*
   * dram_check2()でチェックしたら、値をうまく保持できないビットが存在した
   * ので、リフレッシュレートを上げたら解消された。（ウエイトを多めにしても
   * 効果はなかった）
   * とりあえず、リフレッシュレート高めの設定にしておく
   */
  *H8_3069F_ABWCR   = 0xff;
  *H8_3069F_RTCOR   = 0x03; /* 0x07: リフレッシュ周期を長め */
  *H8_3069F_RTMCSR  = 0x37; /* 0x2f: リフレッシュう周波数UP */
  *H8_3069F_DRCRB   = 0x98; /* 0x9f: ウエイト挿入*/
  *H8_3069F_DRCRA   = 0x30;

  *H8_3069F_P1DDR   = 0xff;
  *H8_3069F_P2DDR   = 0x07;
  *H8_3069F_P8DDR   = 0xe4;
  /* *H8_3069F_PBDDR = ...; */

  /* H8_3069F_WCRH = ...; */
  *H8_3069F_WCRL    = 0xcf;
  *H8_3069F_ASTCR   = 0xfb; /* 0xff: 3ステートアクセス */

  return 0;
}

static int check_val(volatile val_t *p, volatile val_t *wval)
{
  volatile val_t rval;

  p->u.val8[0] = wval->u.val8[0]; p->u.val8[1] = wval->u.val8[1];
  p->u.val8[2] = wval->u.val8[2]; p->u.val8[3] = wval->u.val8[3];
  rval.u.val8[0] = p->u.val8[0]; rval.u.val8[1] = p->u.val8[1];
  rval.u.val8[2] = p->u.val8[3]; rval.u.val8[3] = p->u.val8[3];

  if (rval.u.val32[0] != wval->u.val32[0])
    return -1;

  p->u.val16[0] = wval->u.val16[0];
  p->u.val16[1] = wval->u.val16[2];
  rval.u.val16[0] = p->u.val16[0];
  rval.u.val16[1] = p->u.val16[1];

  if (rval.u.val32[0] != wval->u.val32[0])
    return -1;

  p->u.val32[0] = wval->u.val32[0];
  rval.u.val32[0] = p->u.val32[0];

  if (rval.u.val32[0] != wval->u.val32[0])
    return -1;

  return 0;
}

int dram_check(void)
{
  uint32 *p;
  val_t val;

  puts("DRAM checking...\n");

  for (p = (uint32 *)DRAM_START; p < (uint32 *)DRAM_END; p++) {
    putxval((unsigned long)p, 0);

    val.u.val32[0] = (uint32)p;
    if (check_val((val_t *)p, &val) < 0)
      goto err;

    val.u.val32[0] = 0;
    if (check_val((val_t *)p, &val) < 0)
      goto err;

    val.u.val32[0] = 0xffffffffUL;
    if (check_val((val_t *)p, &val) < 0)
      goto err;

    puts("\x08\x08\x08\x08\x08\x08\x08\x08");
  }
  puts("\nall check OK.\n");
  return 0;

err:
  puts("\nERROR: ");
  putxval((unsigned long)*p, 8);
  puts("\n");
  return -1;
}

static uint32 dram_check2_val0(uint32 *addr) { return  0x55555555UL; }
static uint32 dram_check2_val1(uint32 *addr) { return  0xaaaaaaaaUL; }
static uint32 dram_check2_val2(uint32 *addr) { return  0x00000000UL; }
static uint32 dram_check2_val3(uint32 *addr) { return  0xffffffffUL; }
static uint32 dram_check2_val4(uint32 *addr) { return  (uint32)addr; }
static uint32 dram_check2_val5(uint32 *addr) { return -(uint32)addr; }

int dram_check2(void)
{
  uint32 *p;
  int ret = 0, i;
  uint32 (*getval[])(uint32 *) = {
    dram_check2_val0,
    dram_check2_val1,
    dram_check2_val2,
    dram_check2_val3,
    dram_check2_val4,
    dram_check2_val5,
    NULL
  };

  for (i = 0; getval[i]; i++) {
    puts("DRAM check pattern: ");
    putxval(i, 0);

    puts("\nDRAM setting...\n");

    for (p = (uint32 *)DRAM_START; p < (uint32 *)DRAM_END; p++) {
      *p = getval[i](p);
      if (!((uint32)p & 0xfff)) {
        putxval((unsigned long)p, 8);
        puts("\x08\x08\x08\x08\x08\x08\x08\x08");
      }
    }
    puts("\nDRAM checking...\n");

    for (p = (uint32 *)DRAM_START; p < (uint32 *)DRAM_END; p++) {
      if (*p != getval[i](p)) {
        puts("\nERROR! :");
        putxval((unsigned long)p, 8);
        puts(" ");
        putxval((unsigned long)*p, 8);
        puts("\n");
        ret = -1;
      }
      if (!((uint32)p & 0xfff)) {
        putxval((unsigned long)p, 8);
        puts("\x08\x08\x08\x08\x08\x08\x08\x08");
      }
    }
    puts("\n");
  }

  if (ret == 0) {
    puts("\nall check OK.\n");
  }

  return ret;
}

int dram_clear(void)
{
  uint32 *p;

  puts("DRAM clearing...\n");

  for (p = (uint32 *)DRAM_START; p < (uint32 *)DRAM_END; p++)
    *p = 0;

  puts("DRAM cleared.\n");

  return 0;
}
