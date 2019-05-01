#include "defines.h"
#include "lib.h"
#include "ethernet.h"

/* I/O関係レジスタ定義 */
#define H8_3069F_P1DDR   ((volatile uint8 *)0xfee000)
#define H8_3069F_P3DDR   ((volatile uint8 *)0xfee002)
#define H8_3069F_P6DDR   ((volatile uint8 *)0xfee005)
#define H8_3069F_P8DDR   ((volatile uint8 *)0xfee007)
#define H8_3069F_P9DDR   ((volatile uint8 *)0xfee008)

/* 割込みコントローラ関係レジスタ定義 */
#define H8_3069F_SYSCR   ((volatile uint8 *)0xfee012)
#define H8_3069F_ISCR    ((volatile uint8 *)0xfee014)
#define H8_3069F_IER     ((volatile uint8 *)0xfee015)
#define H8_3069F_ISR     ((volatile uint8 *)0xfee016)
#define H8_3069F_IPRA    ((volatile uint8 *)0xfee018)
#define H8_3069F_IPRB    ((volatile uint8 *)0xfee019)

#define RTL8019_ADDR     0x200000

#define NE2000_CR        ((volatile uint8 *)(RTL8019_ADDR + 0x00)) /* コマンドレジスタ */
#define RTL8019_RDMAP    ((volatile uint8 *)(RTL8019_ADDR + 0x10)) /* リモートDMAポート */
#define RTL8019_RP       ((volatile uint8 *)(RTL8019_ADDR + 0x18)) /* リセットポート */

/* Page0 */
#define NE2000_PSTART    ((volatile uint8 *)(RTL8019_ADDR + 0x01)) /* 受信リングバッファの開始アドレスを設定 */
#define NE2000_PSTOP     ((volatile uint8 *)(RTL8019_ADDR + 0x02)) /* 受信リングバッファの最終アドレスを設定 */
#define NE2000_BNRY      ((volatile uint8 *)(RTL8019_ADDR + 0x03)) /* 受信リングバッファの最終受信アドレスを示す */
#define NE2000_TPSR      ((volatile uint8 *)(RTL8019_ADDR + 0x04)) /* 送信リングバッファの開始アドレスを設定 */
#define NE2000_TBCR0     ((volatile uint8 *)(RTL8019_ADDR + 0x05)) /* 送信パケットのバイト数を設定 */
#define NE2000_TBCR1     ((volatile uint8 *)(RTL8019_ADDR + 0x06))
#define NE2000_ISR       ((volatile uint8 *)(RTL8019_ADDR + 0x07)) /* NIC割込状態を示す */
#define NE2000_RSAR0     ((volatile uint8 *)(RTL8019_ADDR + 0x08)) /* リモートDMAの開始アドレスを設定 */
#define NE2000_RSAR1     ((volatile uint8 *)(RTL8019_ADDR + 0x09))
#define NE2000_RBCR0     ((volatile uint8 *)(RTL8019_ADDR + 0x0a)) /* リモートDMAのバイト数を設定 */
#define NE2000_RBCR1     ((volatile uint8 *)(RTL8019_ADDR + 0x0b))
#define NE2000_RCR       ((volatile uint8 *)(RTL8019_ADDR + 0x0c)) /* 受信設定 */
#define NE2000_TCR       ((volatile uint8 *)(RTL8019_ADDR + 0x0d)) /* 送信設定 */
#define NE2000_DCR       ((volatile uint8 *)(RTL8019_ADDR + 0x0e)) /* データ設定 */
#define NE2000_IMR       ((volatile uint8 *)(RTL8019_ADDR + 0x0f)) /* 割込マスク: ISRの各ビットに対応 */

/* Page1 */
#define NE2000_PAR0      ((volatile uint8 *)(RTL8019_ADDR + 0x01)) /* 自ホストの物理アドレス */
#define NE2000_PAR1      ((volatile uint8 *)(RTL8019_ADDR + 0x02))
#define NE2000_PAR2      ((volatile uint8 *)(RTL8019_ADDR + 0x03))
#define NE2000_PAR3      ((volatile uint8 *)(RTL8019_ADDR + 0x04))
#define NE2000_PAR4      ((volatile uint8 *)(RTL8019_ADDR + 0x05))
#define NE2000_PAR5      ((volatile uint8 *)(RTL8019_ADDR + 0x06))
#define NE2000_CURR      ((volatile uint8 *)(RTL8019_ADDR + 0x07)) /* 第1受信バッファのページアドレスを示す */
#define NE2000_MAR0      ((volatile uint8 *)(RTL8019_ADDR + 0x08))
#define NE2000_MAR1      ((volatile uint8 *)(RTL8019_ADDR + 0x09))
#define NE2000_MAR2      ((volatile uint8 *)(RTL8019_ADDR + 0x0a))
#define NE2000_MAR3      ((volatile uint8 *)(RTL8019_ADDR + 0x0b))
#define NE2000_MAR4      ((volatile uint8 *)(RTL8019_ADDR + 0x0c))
#define NE2000_MAR5      ((volatile uint8 *)(RTL8019_ADDR + 0x0d))
#define NE2000_MAR6      ((volatile uint8 *)(RTL8019_ADDR + 0x0e))
#define NE2000_MAR7      ((volatile uint8 *)(RTL8019_ADDR + 0x0f))

/* Page3 (for RTL8019) */
#define RTL8019_9346CR  ((volatile uint8 *)(RTL8019_ADDR + 0x01))
#define RTL8019_BPAGE   ((volatile uint8 *)(RTL8019_ADDR + 0x02))
#define RTL8019_CONFIG0 ((volatile uint8 *)(RTL8019_ADDR + 0x03))
#define RTL8019_CONFIG1 ((volatile uint8 *)(RTL8019_ADDR + 0x04))
#define RTL8019_CONFIG2 ((volatile uint8 *)(RTL8019_ADDR + 0x05))
#define RTL8019_CONFIG3 ((volatile uint8 *)(RTL8019_ADDR + 0x06))
#define RTL8019_TEST    ((volatile uint8 *)(RTL8019_ADDR + 0x07))
#define RTL8019_CSNSAV  ((volatile uint8 *)(RTL8019_ADDR + 0x08))
#define RTL8019_HLTCLK  ((volatile uint8 *)(RTL8019_ADDR + 0x09))
#define RTL8019_INTR    ((volatile uint8 *)(RTL8019_ADDR + 0x0b))
#define RTL8019_CONFIG4 ((volatile uint8 *)(RTL8019_ADDR + 0x0d))

/* CR: Command Register: 00H in all pages */
#define NE2000_CR_P0       (0 << 6) /* Page0 選択: PS1=0, PS0 = 0 */
#define NE2000_CR_P1       (1 << 6) /* Page1 選択: PS1=0, PS0 = 1 */
#define NE2000_CR_P2       (2 << 6) /* Page2 選択: PS1=1, PS0 = 0 */
#define NE2000_CR_P3       (3 << 6) /* Page3 選択: PS1=1, PS0 = 1 */
#define NE2000_CR_RD_ABORT (4 << 3) /* abort/complete remote DMA */
#define NE2000_CR_RD_WRITE (2 << 3) /* remote write */
#define NE2000_CR_RD_READ  (1 << 3) /* remote read */
#define NE2000_CR_TXP      (1 << 2) /* set to transmit a packet */
#define NE2000_CR_STA      (1 << 1) /* STA = 1, STP = 0 -> Start command */
#define NE2000_CR_STP      (1 << 0) /* STA = 0, STP = 1 -> Stop command  */

/* ISR: Interrupt Status Registor: 07H in Page0 */
#define NE2000_ISR_RDC     (1 << 6) /* remote DMA operation has been completed */
#define NE2000_ISR_PTX     (1 << 1) /* packet transmitted with no errors */
#define NE2000_ISR_PRX     (1 << 0) /* packet received with no errors */

/* RCR: Receive Configuration Register: 0CH in Page0 (W), in Page2 (R) */
#define NE2000_RCR_MON     (1 << 5) /* Monitor mode: check packet */
#define NE2000_RCR_PRO     (1 << 4) /* accept all packet with physical dest-addr */
#define NE2000_RCR_AM      (1 << 3) /* accept packets with multicast dest-addr */
#define NE2000_RCR_AB      (1 << 2) /* accecpt packets with broadcat dest-addr */

/* TCR: Transmit Configuration Register: 0D in Page0 (W), in Page2 (R) */
#define NE2000_TCR_ELB     (2 << 1) /* External lookback */
#define NE2000_TCR_ILB     (1 << 1) /* Internal lookback */
#define NE2000_TCR_NORMAL  (0 << 1) /* Normal operation  */

/* DCR: Data Configuration Register: 0EH in Pageo (W), in Page2 (R) */
#define NE2000_DCR_F1      (1 << 6) /* select FIFO threshold:  FT1 */
#define NE2000_DCR_F0      (1 << 5) /* select FIFO threshold:  FT0 */
#define NE2000_DCR_LS      (1 << 3) /* Loopback select, 0: loopback mode */
#define NE2000_DCR_BOS     (1 << 1) /* Byte order select, 0: Little, 1: Big */

#define NE2000_TP_START    0x40     /* start page address of packet to the transmitted */
#define NE2000_RP_START    0x46     /* start page address of received buffer ring */
#define NE2000_RP_STOP     0x80     /* stop page address of received buffer ring */

static void udelay(int usec)
{
  volatile int i, j;
  for (j = 0; j < usec; j++)
    for (i = 0; i < 20; i++)  /* 20MHz: 1Hz = 1/20 usec */
      ;
}

static void mdelay(int msec)
{
  volatile int i;
  for (i = 0; i < msec; i++) {
    udelay(1000);
  }
}

static int read_data(int addr, int size, char *buf)
{
  int i;

  *NE2000_CR    = NE2000_CR_P0 | NE2000_CR_RD_ABORT | NE2000_CR_STA;
  *NE2000_RBCR0 = size & 0xff;
  *NE2000_RBCR1 = (size >> 8) & 0xff;
  *NE2000_RSAR0 = addr & 0xff;
  *NE2000_RSAR1 = (addr >> 8) & 0xff;
  *NE2000_ISR   = NE2000_ISR_RDC;
  *NE2000_CR    = NE2000_CR_P0 | NE2000_CR_RD_READ | NE2000_CR_STA;
  for (i = 0; i < size; i++) { /* 受信処理: リモートDMAポートから読み込む */
    buf[i] = *RTL8019_RDMAP;
  }
  while ((*NE2000_ISR & NE2000_ISR_RDC) == 0) /* 受信完了を待つ */
    ;

  return 0;
}

static int write_data(int addr, int size, char *buf)
{
  int i;

  *NE2000_CR    = NE2000_CR_P0 | NE2000_CR_RD_ABORT | NE2000_CR_STA;
  *NE2000_RBCR0 = size & 0xff;
  *NE2000_RBCR1 = (size >> 8) & 0xff;
  *NE2000_RSAR0 = addr & 0xff;
  *NE2000_RSAR1 = (addr >> 8) & 0xff;
  *NE2000_ISR   = NE2000_ISR_RDC;
  *NE2000_CR    = NE2000_CR_P0 | NE2000_CR_RD_WRITE | NE2000_CR_STA;
  for (i = 0; i < size; i++) { /* 送信処理: リモートDMAポートに書き込む */
    *RTL8019_RDMAP = buf[i];
  }
  while ((*NE2000_ISR & NE2000_ISR_RDC) == 0) /* 受信完了を待つ */
    ;

  return 0;
}

static int port_init(void)
{
#if 0 /* DRAMで設定しているので不要 */
  *H8_3069F_P1DDR = 0x1f; /* A0 - A4 */
  *H8_3069F_P3DDR = ...;  /* モード5では D8-D15 の設定は不要 */
  *H8_3069F_P6DDR = ...;  /* モード5では HWR, RD の設定は不要 */
#endif

  *H8_3069F_P8DDR = 0xec; /* 11101100: CS1(ether) and CS2(DRAM) */

#if 0
  *H8_3069F_P9DDR = ...;  /* IERの設定によるので設定不要 */
#endif

  *H8_3069F_ISCR = 0x00;  /* lowレベルで割込み */
  *H8_3069F_IPRA = 0x00;

  return 0;
}

int rtl8019_init(int index, unsigned char macaddr[])
{
  unsigned char t1;
  unsigned char t2[12];
  int i;

  port_init();

  t1 = *RTL8019_RP;
  *RTL8019_RP = t1;

  mdelay(10);

  *NE2000_CR     = NE2000_CR_P0 | NE2000_CR_RD_ABORT | NE2000_CR_STP;
  *NE2000_DCR    = NE2000_DCR_F1 | NE2000_DCR_LS | NE2000_DCR_BOS;
  *NE2000_RBCR0  = 0x00;
  *NE2000_RBCR1  = 0x00;
  *NE2000_RCR    = NE2000_RCR_MON;
  *NE2000_TCR    = NE2000_TCR_ILB;
  *NE2000_TPSR   = NE2000_TP_START;
  *NE2000_PSTART = NE2000_RP_START;
  *NE2000_BNRY   = NE2000_RP_START;
  *NE2000_PSTOP  = NE2000_RP_STOP;
  *NE2000_IMR    = 0x00;
  *NE2000_ISR    = 0xff;

  read_data(0, 12, t2);
  puts("MAC: ");
  for (i = 0; i < 6; i++) {
    macaddr[i] = t2[i * 2];
    putxval((unsigned char)macaddr[i], 2);
  }
  puts("\n");

  *NE2000_CR   = NE2000_CR_P1 | NE2000_CR_RD_ABORT | NE2000_CR_STP;
  *NE2000_PAR0 = macaddr[0];
  *NE2000_PAR1 = macaddr[1];
  *NE2000_PAR2 = macaddr[2];
  *NE2000_PAR3 = macaddr[3];
  *NE2000_PAR4 = macaddr[4];
  *NE2000_PAR5 = macaddr[5];
  *NE2000_CURR = NE2000_RP_START + 1;
  *NE2000_MAR0 = 0x00;
  *NE2000_MAR1 = 0x00;
  *NE2000_MAR2 = 0x00;
  *NE2000_MAR3 = 0x00;
  *NE2000_MAR4 = 0x00;
  *NE2000_MAR5 = 0x00;
  *NE2000_MAR6 = 0x00;
  *NE2000_MAR7 = 0x00;

#if 0
  *NE2000_CR   = NE2000_CR_P3 | NE2000_CR_RD_ABORT | NE2000_CR_STP;
  *RTL8019_9346CR  = 0xc0;
  *RTL8019_CONFIG1 = 0x80 | 0x40;
  *RTL8019_CONFIG2 = 0x00;
  *RTL8019_9346CR  = 0x00;
#endif

  *NE2000_CR  = NE2000_CR_P0 | NE2000_CR_RD_ABORT | NE2000_CR_STP;
  *NE2000_RCR = NE2000_RCR_AM | NE2000_RCR_AB | NE2000_RCR_PRO;
  *NE2000_CR  = NE2000_CR_P0 | NE2000_CR_RD_ABORT | NE2000_CR_STA;
  *NE2000_TCR = NE2000_TCR_NORMAL;
  *NE2000_IMR = 0x00;

  /*
   * port_init() の内部でこれをやると，割り込みが残ったままリセットで再起動
   * したときに固まってしまう．
   * RTL8019の割り込みまわりを初期化した後で有効化すること．
   */
  *H8_3069F_IER  = 0x20;  /* IRQ5 割込み有効化 */

  return 0;
}

int rtl8019_intr_is_send_enable(int index)
{
  unsigned char mask;

  *NE2000_CR  = NE2000_CR_P2 | NE2000_CR_RD_ABORT | NE2000_CR_STA;
  mask        = *NE2000_IMR;
  *NE2000_CR  = NE2000_CR_P0 | NE2000_CR_RD_ABORT | NE2000_CR_STA;
  return (mask & NE2000_ISR_PTX);
}

void rtl8019_intr_send_enable(int index)
{
  unsigned char mask;

  *NE2000_CR  = NE2000_CR_P2 | NE2000_CR_RD_ABORT | NE2000_CR_STA;
  mask        = *NE2000_IMR;
  *NE2000_CR  = NE2000_CR_P0 | NE2000_CR_RD_ABORT | NE2000_CR_STA;
  *NE2000_IMR = mask | NE2000_ISR_PTX;
}

void rtl8019_intr_send_disable(int index)
{
  unsigned char mask;

  *NE2000_CR  = NE2000_CR_P2 | NE2000_CR_RD_ABORT | NE2000_CR_STA;
  mask        = *NE2000_IMR;
  *NE2000_CR  = NE2000_CR_P0 | NE2000_CR_RD_ABORT | NE2000_CR_STA;
  *NE2000_IMR = mask & ~NE2000_ISR_PTX;
}

int rtl8019_intr_is_recv_enable(int index)
{
  unsigned char mask;

  *NE2000_CR  = NE2000_CR_P2 | NE2000_CR_RD_ABORT | NE2000_CR_STA;
  mask        = *NE2000_IMR;
  *NE2000_CR  = NE2000_CR_P0 | NE2000_CR_RD_ABORT | NE2000_CR_STA;
  return (mask & NE2000_ISR_PRX);
}

void rtl8019_intr_recv_enable(int index)
{
  unsigned char mask;

  *NE2000_CR  = NE2000_CR_P2 | NE2000_CR_RD_ABORT | NE2000_CR_STA;
  mask        = *NE2000_IMR;
  *NE2000_CR  = NE2000_CR_P0 | NE2000_CR_RD_ABORT | NE2000_CR_STA;
  *NE2000_IMR = mask | NE2000_ISR_PRX;
}

void rtl8019_intr_recv_disable(int index)
{
  unsigned char mask;

  *NE2000_CR = NE2000_CR_P2 | NE2000_CR_RD_ABORT | NE2000_CR_STA;
  mask       = *NE2000_IMR;
  *NE2000_CR = NE2000_CR_P0 | NE2000_CR_RD_ABORT | NE2000_CR_STA;
  *NE2000_IMR = mask & ~NE2000_ISR_PRX;
}

int rtl8019_is_send_enable(int index)
{
  unsigned char status;

  *NE2000_CR  = NE2000_CR_P0 | NE2000_CR_RD_ABORT | NE2000_CR_STA;
  status = *NE2000_ISR;
  return ((status & NE2000_ISR_PTX) ? 1 : 0);
}

int rtl8019_is_recv_enable(int index)
{
  unsigned char status;

  *NE2000_CR  = NE2000_CR_P0 | NE2000_CR_RD_ABORT | NE2000_CR_STA;
  status = *NE2000_ISR;
  return ((status & NE2000_ISR_PRX) ? 1 : 0);
}


int rtl8019_intr_clear(int index)
{
  *H8_3069F_ISR = 0x00;
  return 0;
}

int rtl8019_intr_clear_send(int index)
{
  *NE2000_CR  = NE2000_CR_P0 | NE2000_CR_RD_ABORT | NE2000_CR_STA;
  *NE2000_ISR = NE2000_ISR_PTX;
  return 0;
}

int rtl8019_intr_clear_recv(int index)
{
  *NE2000_CR  = NE2000_CR_P0 | NE2000_CR_RD_ABORT | NE2000_CR_STA;
  *NE2000_ISR = NE2000_ISR_PRX;
  return 0;
}

int rtl8019_recv(int index, char *buf)
{
  unsigned char start, curr;
  unsigned char header[4];
  int size;

  *NE2000_CR    = NE2000_CR_P0 | NE2000_CR_RD_ABORT | NE2000_CR_STA;
  start = *NE2000_BNRY + 1;

  *NE2000_CR    = NE2000_CR_P1 | NE2000_CR_RD_ABORT | NE2000_CR_STA;
  curr  = *NE2000_CURR;

  if (curr < start)
    curr += NE2000_RP_STOP - NE2000_RP_START;
  if (start == curr)
    return 0;
  if (start == NE2000_RP_STOP)
    start = NE2000_RP_START;

  read_data(start * 256, 4, header);

  size = ((int)header[3] << 8) + header[2] - 4;
  read_data((start * 256) + 4, size, buf);

  *NE2000_CR = NE2000_CR_P0 | NE2000_CR_RD_ABORT | NE2000_CR_STA;
  if (header[1] == NE2000_RP_START)
    header[1] = NE2000_RP_STOP;
  *NE2000_BNRY = header[1] - 1;

  return size;
}

int rtl8019_send(int index, int size, char *buf)
{
  write_data(NE2000_TP_START * 256, size, buf);

  /* Ethernetフレームの最小長 = 60 = ethernet_header(14) + IPデータグラム(46-1500) */
  if (size < 60)
    size = 60;

  *NE2000_CR    = NE2000_CR_P0 | NE2000_CR_RD_ABORT | NE2000_CR_STA;
  *NE2000_TBCR0 = size & 0xff;
  *NE2000_TBCR1 = (size >> 8) & 0xff;
  *NE2000_TPSR  = NE2000_TP_START;
  *NE2000_CR    = NE2000_CR_P0 | NE2000_CR_RD_ABORT | NE2000_CR_TXP | NE2000_CR_STA;
  while ((*NE2000_CR & NE2000_CR_TXP) != 0)
    ;

  return 0;
}
