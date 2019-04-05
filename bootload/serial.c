#include "defines.h"
#include "serial.h"

#define SERIAL_SCI_NUM  3       /* SCIの数 */

#define H8_3069F_SCI0 ((volatile struct h8_3069f_sci *) 0xffffb0)   /* HM 13.1.4 (13-4) */
#define H8_3069F_SCI1 ((volatile struct h8_3069f_sci *) 0xffffb8)
#define H8_3069F_SCI2 ((volatile struct h8_3069f_sci *) 0xffffc0)

struct h8_3069f_sci {   /* HM 13.1.4 (13-4)             */
  volatile uint8 smr;   /* シリアルモードレジスタ       */
  volatile uint8 brr;   /* ビットレートレジスタ         */
  volatile uint8 scr;   /* シリアルコントロールレジスタ */
  volatile uint8 tdr;   /* トランスミットデータレジスタ */
  volatile uint8 ssr;   /* シリアルステータスレジスタ   */
  volatile uint8 rdr;   /* レシーブデータレジスタ       */
  volatile uint8 scmr;  /* スマートカードモードレジスタ */
};

/* SMR: HM 13.2.5 (13-7/10) */
#define H8_3069F_SCI_SMR_CKS_PER1   (0<<0)  /* BIT 0-1 クロックセレクタ */
#define H8_3069F_SCI_SMR_CKS_PER4   (1<<0)
#define H8_3069F_SCI_SMR_CKS_PER16  (2<<0)
#define H8_3069F_SCI_SMR_CKS_PER64  (3<<0)
#define H8_3069F_SCI_SMR_MP   (1<<2)        /* BIT 2  マルチプロセッサ, 0: 禁止, 1: 選択 */
#define H8_3069F_SCI_SMR_STOP (1<<3)        /* BIT 3  ストップビット,   0: 1bit, 1: 2bit */
#define H8_3069F_SCI_SMR_OE   (1<<4)        /* BIT 4  パリティ,         0: 偶数, 1: 奇数 */
#define H8_3069F_SCI_SMR_PE   (1<<5)        /* BIT 5  パリティ,         0: 禁止, 1: 許可 */
#define H8_3069F_SCI_SMR_CHR  (1<<6)        /* BIT 6  データ長,         0: 8bit, 1: 7bit */
#define H8_3069F_SCI_SMR_CA   (1<<7)        /* BIT 7  動作モード        0: 調歩同期      */

/* SCR: HM 13.2.6 (13-11/14) */
#define H8_3069F_SCI_SCR_CKE0   (1<<0)      /* BIT 0  クロックイネーブル 0 */
#define H8_3069F_SCI_SCR_CKE1   (1<<1)      /* BIT 1  クロックイネーブル 1*/
#define H8_3069F_SCI_SCR_TEIE   (1<<2)      /* BIT 2  送信終了割込要求      0: 禁止, 1: 許可 */
#define H8_3069F_SCI_SCR_MPIE   (1<<3)      /* BIT 3  マルチプロセッサ割込  0: 禁止, 1: 許可 */
#define H8_3069F_SCI_SCR_RE     (1<<4)      /* BIT 4  受信                  0: 禁止, 1: 許可 */
#define H8_3069F_SCI_SCR_TE     (1<<5)      /* BIT 5  送信                  0: 禁止, 1: 許可 */
#define H8_3069F_SCI_SCR_RIE    (1<<6)      /* BIT 6  受信割込要求          0: 禁止, 1: 許可 */
#define H8_3069F_SCI_SCR_TIE    (1<<7)      /* BIT 7  僧院割込要求          0: 禁止, 1: 許可 */

/* SSR: HM 13.2.7 (13-15/20) */
#define H8_3069F_SCI_SSR_MPBT   (1<<0)      /* BIT 0  送信マルチプロセッサビット 0: 0, 1: 1 */
#define H8_3069F_SCI_SSR_MPB    (1<<1)      /* BIT 1  受信マルチプロセッサビット 0: 0, 1: 1 */
#define H8_3069F_SCI_SSR_TEND   (1<<2)      /* BIT 2  送信               0: 送信中, 1: 終了 */
#define H8_3069F_SCI_SSR_PER    (1<<3)      /* BIT 3  パリティエラー     0: 正常, 1: エラー */
#define H8_3069F_SCI_SSR_FERERS (1<<4)      /* BIT 4  フレーミングエラー 0: 正常, 1: エラー */
#define H8_3069F_SCI_SSR_ORER   (1<<5)      /* BIT 5  オーバーランエラー 0: 正常, 1: エラー */
#define H8_3069F_SCI_SSR_RDRF   (1<<6)      /* BIT 6  受信完了   0: 受信データなし, 1: あり */
#define H8_3069F_SCI_SSR_TDRE   (1<<7)      /* BIT 7  送信完了   0: 送信データあり, 1: なし */

static struct {
  volatile struct h8_3069f_sci *sci;
} regs[SERIAL_SCI_NUM] = {
  { H8_3069F_SCI0 },
  { H8_3069F_SCI1 },
  { H8_3069F_SCI2 },
};

/* デバイス初期化 */
int serial_init(int index)
{
  volatile struct h8_3069f_sci *sci = regs[index].sci;

  sci->scr = 0;
  sci->smr = 0;
  sci->brr = 15;      /* 20MHzのクロックから38400bpsを生成 */
  sci->scr = H8_3069F_SCI_SCR_RE | H8_3069F_SCI_SCR_TE;   /* 送受信可能 */
  sci->ssr = 0;

  return 0;
}

/* 送信可能か？ */
int serial_is_send_enable(int index)
{
  volatile struct h8_3069f_sci *sci = regs[index].sci;
  return (sci->ssr & H8_3069F_SCI_SSR_TDRE);
}

/* 一文字送信 */
int serial_send_byte(int index, unsigned char c)
{
  volatile struct h8_3069f_sci *sci = regs[index].sci;

  /* 送信可能になるまで待つ */
  while(!serial_is_send_enable(index))
    ;
  sci->tdr = c;
  sci->ssr &= ~H8_3069F_SCI_SSR_TDRE;   /* 送信開始 */

  return 0;
}

/* 受信可能か？ */
int serial_is_recv_enable(int index)
{
  volatile struct h8_3069f_sci *sci = regs[index].sci;

  return (sci->ssr & H8_3069F_SCI_SSR_RDRF);
}

/* 一文字受信 */
unsigned char serial_recv_byte(int index)
{
  volatile struct h8_3069f_sci *sci = regs[index].sci;
  unsigned char c;

  /* 受信文字が来るまで待つ */
  while (!serial_is_recv_enable(index))
    ;

  c = sci->rdr;
  sci->ssr &= ~H8_3069F_SCI_SSR_RDRF; /* 受信完了 */

  return c;
}
