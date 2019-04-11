#include "defines.h"
#include "kozos.h"
#include "intr.h"
#include "interrupt.h"
#include "rtl8019.h"
#include "ethernet.h"
#include "lib.h"
#include "netdrv.h"

static unsigned char my_macaddr[MACADDR_SIZE];

static struct netbuf *send_buf_head;
static struct netbuf **send_buf_tail = &send_buf_head;

static void recv_packet(void)
{
  struct netbuf *pkt;

  while (1) {
    pkt = kz_kmalloc(DEFAULT_NETBUF_SIZE);
    memset(pkt, 0, DEFAULT_NETBUF_SIZE);
    pkt->cmd = ETHERNET_CMD_RECV;

    /*
     * ethernetフレームが14バイトで４の倍数でないので，ethernetフレーム以降が
     * ４バイトアラインメントされるように，データの先頭を２バイト空ける．
     */
    pkt->top = pkt->data + 2;

    pkt->size = rtl8019_recv(0, pkt->top);
    if (pkt->size > 0) {
      kz_send(MSGBOX_ID_ETHPROC, 0, (char *)pkt);
    } else {
      kz_kmfree(pkt);
      break;
    }
  }
}

static void send_packet(struct netbuf *buf)
{
  /* 終端に追加 */
  if (buf) {
    buf->next = NULL;
    *send_buf_tail = buf;
    send_buf_tail = &buf->next;
  }

  if (send_buf_head) {
    if (!buf || !rtl8019_intr_is_send_enable(0)) {
      /* 先頭から抜き出す */
      buf = send_buf_head;
      send_buf_head = send_buf_head->next;
      if (!send_buf_head)
        send_buf_tail = &send_buf_head;
      buf->next = NULL;

      /*
       * 送信割込み無効ならば，送信開始されていないので送信開始する．
       * 送信割込み有効ならば送信開始されており，送信割込みの延長で
       * 送信バッファ内のデータが順次送信されるので，何もしなくてよい．
       */
      if (!rtl8019_intr_is_send_enable(0)) {
        rtl8019_intr_send_enable(0);  /* 送信割込み有効化 */
      }

      rtl8019_send(0, buf->size, buf->top);
      kz_kmfree(buf);
    }
  }
}

/* 割込みハンドラ */
static void netdrv_intr(void)
{
  struct netbuf *pkt;

  /* send/recv個別にクリアするので不要?     */
  /* コメントアウトしないと受信が始まらない */
  /* rtl8019_intr_clear(0);                 */


  if (rtl8019_is_recv_enable(0)) {
    rtl8019_intr_clear_recv(0);
    pkt = kx_kmalloc(DEFAULT_NETBUF_SIZE);
    memset(pkt, 0, DEFAULT_NETBUF_SIZE);
    pkt->cmd = NETDRV_CMD_RECVINTR;
    kx_send(MSGBOX_ID_NETPROC, 0, (char *)pkt);
  }

  if (rtl8019_is_send_enable(0)) {
    rtl8019_intr_clear_send(0);
    pkt = kx_kmalloc(DEFAULT_NETBUF_SIZE);
    memset(pkt, 0, DEFAULT_NETBUF_SIZE);
    pkt->cmd = NETDRV_CMD_SENDINTR;
    kx_send(MSGBOX_ID_NETPROC, 0, (char *)pkt);
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
  int ret = 0;

  switch (buf->cmd) {
    case NETDRV_CMD_USE:  /* イーサネットドライバの使用開始 */
      rtl8019_intr_recv_enable(0); /* 受信割込み有効化（受信開始） */
      buf = kz_kmalloc(sizeof(*buf));
      buf->cmd = ETHERNET_CMD_MACADDR;
      memcpy(buf->option.common.macaddr.addr, my_macaddr, MACADDR_SIZE);
      kz_send(MSGBOX_ID_ETHPROC, 0, (char *)buf);
      break;

    case NETDRV_CMD_SEND: /* イーサネットへのフレーム出力 */
      /* 割込禁止にする必要は無い */
      send_packet(buf);
      ret = 1;
      break;

    case NETDRV_CMD_RECVINTR: /* イーサネットからのフレーム受信 */
      /* 割込禁止にする必要は無い */
      recv_packet();
      break;

    case NETDRV_CMD_SENDINTR: /* イーサネットへのフレーム出力完了 */
      /* 割込禁止にする必要は無い */
      if (!send_buf_head) {
        /* 送信データが無いならば，送信処理終了 */
        rtl8019_intr_send_disable(0);
      } else {
        /* 送信データがあるならば，引続き送信する */
        send_packet(NULL);
      }
      break;

    default:
      break;
  }

  return ret;
}

int netdrv_main(int argc, char *argv[])
{
  struct netbuf *buf;
  int ret;

  netdrv_init();
  kz_setintr(SOFTVEC_TYPE_ETHINTR, netdrv_intr);  /* 割込みハンドラ設定 */

  while (1) {
    kz_recv(MSGBOX_ID_NETPROC, NULL, (char **)&buf);
    ret = netdrv_proc(buf);
    if (!ret) kz_kmfree(buf);
  }

  return 0;
}
