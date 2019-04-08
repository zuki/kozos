#include "defines.h"
#include "kozos.h"
#include "netdrv.h"
#include "ethernet.h"
#include "arp.h"
#include "tcp.h"
#include "ip.h"
#include "lib.h"

struct ip_header {
  uint8  v_hl;
  uint8  tos;
  uint16 length; /* IPヘッダ以降のパケットサイズ(IPヘッダを含む) */
  uint16 id;
  uint16 fragment;
  uint8  ttl;
  uint8  protocol;
  uint16 checksum;
  uint32 src_addr;
  uint32 dst_addr;
};

#define IPADDR  0xc0a80b0b   /* 192.168.11.11 */
static uint32 my_ipaddr = IPADDR;

#define PROTOCOL_MAXNUM IP_PROTOCOL_UDP   /* UDPまで利用可能 */

struct protoinfo {
  unsigned char cmd;
  kz_msgbox_id_t id;
} protoinfo[PROTOCOL_MAXNUM + 1];

uint16 ip_calc_checksum(int size, void *buf)
{
  int i;
  uint16 val;
  uint32 sum = 0;
  uint16 *p = buf;

  for (i = 0; i < size; i += sizeof(uint16)) {
    if (size - i == 1)
      val = *(unsigned char *)p << 8;
    else
      val = *p;
    sum += val;
    p++;
  }

  while (sum > 0xffff)
    sum = ((sum >> 16) & 0xffff) + (sum & 0xffff);

  return ~sum & 0xffff;
}

static int ip_recv(struct netbuf *pkt)
{
  struct ip_header *iphdr;
  int hdrlen;

  iphdr = (struct ip_header *)pkt->top;

  if (((iphdr->v_hl >> 4) & 0xf) != 4)
    return 0;
  if (iphdr->dst_addr != my_ipaddr)
    return 0;
  if (iphdr->protocol > PROTOCOL_MAXNUM) /* 未サポートのプロトコル */
    return 0;
  if (!protoinfo[iphdr->protocol].id) /* 登録されていないプロトコル */
    return 0;

  hdrlen = (iphdr->v_hl & 0xf) << 2;

  if (pkt->size > iphdr->length)
    pkt->size = iphdr->length;

  pkt->top  += hdrlen;
  pkt->size -= hdrlen;

  pkt->cmd = protoinfo[iphdr->protocol].cmd;
  pkt->option.common.ipaddr.addr = iphdr->src_addr;
  kz_send(protoinfo[iphdr->protocol].id, 0, (char *)pkt);
  return 1;
}

static int ip_send(struct netbuf *pkt)
{
  struct ip_header *iphdr;
  static int id = 0;
  int hdrlen;

  hdrlen = sizeof(struct ip_header);

  pkt->top  -= hdrlen;
  pkt->size += hdrlen;

  iphdr = (struct ip_header *)pkt->top;

  iphdr->v_hl     = (4 << 4) | (hdrlen >> 2);
  iphdr->tos      = 0;
  iphdr->length   = pkt->size;
  iphdr->id       = id++;
  iphdr->fragment = 0;
  iphdr->ttl      = 64;
  iphdr->protocol = pkt->option.ip.send.protocol;

  iphdr->src_addr = my_ipaddr;
  iphdr->dst_addr = pkt->option.ip.send.dst_addr;

  iphdr->checksum = 0;
  iphdr->checksum = ip_calc_checksum(hdrlen, iphdr);

  pkt->cmd = ETHERNET_CMD_SEND;
  memset(pkt->option.ethernet.send.dst_macaddr, 0, MACADDR_SIZE);
  pkt->option.ethernet.send.type = ETHERNET_TYPE_IP;
  pkt->option.ethernet.send.dst_ipaddr = iphdr->dst_addr;
  kz_send(MSGBOX_ID_ETHPROC, 0, (char *)pkt);

  return 1;
}

static int ip_proc(struct netbuf *buf)
{
  struct protoinfo *info;
  int ret = 0;

  switch (buf->cmd) {
    case IP_CMD_REGPROTO:
      info = &protoinfo[buf->option.ip.regproto.protocol];
      info->cmd = buf->option.ip.regproto.cmd;
      info->id  = buf->option.ip.regproto.id;
      break;
    case IP_CMD_RECV:
      ret = ip_recv(buf);
      break;
    case IP_CMD_SEND:
      ret = ip_send(buf);
      break;
    default:
      break;
  }

  return ret;
}

int ip_main(int argc, char *argv[])
{
  struct netbuf *buf;
  int ret;

  buf = kz_kmalloc(sizeof(*buf));
  buf->cmd = ARP_CMD_IPADDR;
  buf->option.common.ipaddr.addr = IPADDR;
  kz_send(MSGBOX_ID_ARPPROC, 0, (char *)buf);

  buf = kz_kmalloc(sizeof(*buf));
  buf->cmd = TCP_CMD_IPADDR;
  buf->option.common.ipaddr.addr = IPADDR;
  kz_send(MSGBOX_ID_TCPPROC, 0, (char *)buf);

  while (1) {
    kz_recv(MSGBOX_ID_IPPROC, NULL, (char **)&buf);
    ret = ip_proc(buf);
    if (!ret) kz_kmfree(buf);
  }

  return 0;
}
