#ifndef _NETDRV_H_INCLUDED_
#define _NETDRV_H_INCLUDED_

#define NETDRV_DEVICE_NUM 1
#define NETDRV_CMD_USE    'u'   /* イーサネット・ドライバの使用開始 */
#define NETDRV_CMD_SEND   's'   /* イーサネットへのフレーム出力     */

#define MACADDR_SIZE  6
#define IPADDR_SIZE   4

#define ETHERNET_TYPE_IP    0x0800
#define ETHERNET_TYPE_ARP   0x0806

#define IP_PROTOCOL_ICMP    1
#define IP_PROTOCOL_TCP     6
#define IP_PROTOCOL_UDP    17

#define DEFAULT_NETBUF_SIZE 1800

/* パケットとかMACアドレス情報とかをやり取りするための構造体 */
struct netbuf {
  unsigned char cmd;
  short size;
  struct netbuf *next;

  union {
    union {
      struct {
        uint8 addr[MACADDR_SIZE];
      } macaddr;
      struct {
        uint32 addr;
      } ipaddr;
    } common;
    union {
      struct {
        uint8  dst_macaddr[MACADDR_SIZE];
        uint16 type;
        uint32 dst_ipaddr;
      } send;
    } ethernet;
    union {
      struct {
        uint8  protocol;
        unsigned char cmd;
        kz_msgbox_id_t id;
      } regproto;
      struct {
        uint8  protocol;
        uint32 dst_addr;
      } send;
    } ip;
    union {
      struct {
        int    number;
        uint32 ipaddr;
      } send;
    } icmp;
    union {
      struct {
        uint16 port;
        kz_msgbox_id_t id;
      } accept;
      struct {
        uint16 port;
        uint32 ipaddr;
        kz_msgbox_id_t id;
      } connect;
      struct {
        int number;
      } establish;
      struct {
        int number;
      } close;
      struct {
        int number;
      } send;
    } tcp;
  } option;

  /*
   * unionのサイズが２の倍数でもデータの先頭が確実に４バイトアライン
   * されるように，ここでポインタを定義する．
   */
  char *top;  /* データの先頭を指すポインタ */

  char data[0];
};

#endif
