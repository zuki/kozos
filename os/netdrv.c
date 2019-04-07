#include "defines.h"
#include "kozos.h"
#include "intr.h"
#include "interrupt.h"
#include "rtl8019.h"
#include "ethernet.h"
#include "lib.h"
#include "netdrv.h"

static unsigned char my_macaddr[MACADDR_SIZE];

/* 割込みハンドラ */
static void netdrv_intr(void)
{
  struct netbuf *pkt;

  if (rtl8019_checkintr(0)) {
    while (1) {
      pkt = kx_kmalloc(DEFAULT_NETBUF_SIZE);
      memset(pkt, 0, DEFAULT_NETBUF_SIZE);
      pkt->cmd = ETHERNET_CMD_RECV;

      /*
       * ethernetフレームが14バイトで４の倍数でないので
       * ethernetフレーム以降が ４バイトアラインメント
       * されるように，データの先頭を２バイト空ける．
       */
      pkt->top = pkt->data + 2;

      pkt->size = rtl8019_recv(0, pkt->top);
      if (pkt->size > 0) {
        kx_send(MSGBOX_ID_ETHPROC, 0, (char *)pkt);
      } else {
        kx_kmfree(pkt);
        break;
      }
    }
    rtl8019_clearintr(0);
  }
}

static int netdrv_init(void)
{
  rtl8019_init(0, my_macaddr);
  return 0;
}

/* スレッドからの要求を処理する */
static int netdrv_proc(struct netbuf *buf)
{
  switch (buf->cmd) {
    case NETDRV_CMD_USE:  /* イーサネットドライバの使用開始 */
      rtl8019_init(0, my_macaddr);
      rtl8019_intr_enable(0); /* 受信割込み有効化（受信開始） */
      buf = kz_kmalloc(sizeof(*buf));
      buf->cmd = ETHERNET_CMD_MACADDR;
      memcpy(buf->option.common.macaddr.addr, my_macaddr, MACADDR_SIZE);
      kz_send(MSGBOX_ID_ETHPROC, 0, (char *)buf);
      break;

    case NETDRV_CMD_SEND: /* イーサネットへのフレーム出力 */
      rtl8019_send(0, buf->size, buf->top);
      break;

    default:
      break;
  }

  return 0;
}

int netdrv_main(int argc, char *argv[])
{
  struct netbuf *buf;

  netdrv_init();
  kz_setintr(SOFTVEC_TYPE_ETHINTR, netdrv_intr);  /* 割込みハンドラ設定 */

  while (1) {
    kz_recv(MSGBOX_ID_NETPROC, NULL, (char **)&buf);
    netdrv_proc(buf);
    kz_kmfree(buf);
  }

  return 0;
}
