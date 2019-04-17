#include "kozos.h"
#include "netdrv.h"
#include "ip.h"
#include "udp.h"
#include "lib.h"

struct portinfo {
  kz_msgbox_id_t  id;
  uint32  src_ipaddr;
  uint32  dst_ipaddr;
  uint16  src_port;
  uint16  dst_port;
};

static uint32 my_ipaddr;

static struct portinfo *udp_search_portinfo_from_addr(
  uint32 src_ipaddr, uint32 dst_ipaddr,
  uint16 src_port, uint16 dst_port)
{
  struct portinfo *pinfo, *ret = NULL;

  while (1) {
    kz_recv(MSGBOX_ID_UDPPORTLIST, NULL, (char **)&pinfo);
    if (pinfo == NULL) {
      kz_send(MSGBOX_ID_UDPPORTLIST, 0, NULL);
      break;
    }
    if (!src_ipaddr || (pinfo->src_ipaddr == src_ipaddr)) {
      if (!dst_ipaddr || (pinfo->dst_ipaddr == dst_ipaddr))
        if (!src_port || (pinfo->src_port == src_port))
          if (!dst_port || (pinfo->dst_port == dst_port))
            ret = pinfo;
    }
    kz_send(MSGBOX_ID_UDPPORTLIST, 0, (char *)pinfo);
  }
  return ret;
}

static struct portinfo *udp_delete_portinfo(uint16 port)
{
  struct portinfo *pinfo, *ret = NULL;

  while (1) {
    kz_recv(MSGBOX_ID_UDPPORTLIST, NULL, (char **)&pinfo);
    if (pinfo == NULL) {
      kz_send(MSGBOX_ID_UDPPORTLIST, 0, NULL);
      break;
    }
    if (pinfo->src_port == port) {
      ret = pinfo;
    } else {
      kz_send(MSGBOX_ID_UDPPORTLIST, 0, (char *)pinfo);
    }
  }
  return ret;
}

static int udp_free_portinfo(struct portinfo *pinfo)
{
  memset(pinfo, 0, sizeof(*pinfo));
  kz_kmfree(pinfo);

  return 0;
}

static uint16 udp_calc_pseudosum(uint32 src_ipaddr, uint32 dst_ipaddr,
  uint8 protocol, uint16 size)
{
  struct pseudo_header { /* UDPチェックサム計算用の疑似ヘッダ */
    uint32 src_ipaddr;
    uint32 dst_ipaddr;
    uint8  zero;
    uint8  protocol;
    uint16 size;
  } phdr;

  phdr.src_ipaddr = src_ipaddr;
  phdr.dst_ipaddr = dst_ipaddr;
  phdr.zero       = 0;
  phdr.protocol   = protocol;
  phdr.size       = size;

  return ~ip_calc_checksum(sizeof(phdr), &phdr);
}

static int udp_sendpkt(struct netbuf *pkt, uint32 src_ipaddr, uint32 dst_ipaddr)
{
  struct udp_header *udphdr;

  udphdr = (struct udp_header *)pkt->top;

  udphdr->checksum = udp_calc_pseudosum(src_ipaddr, dst_ipaddr, IP_PROTOCOL_UDP, pkt->size);
  udphdr->checksum = ip_calc_checksum(pkt->size, udphdr);

  pkt->cmd = IP_CMD_SEND;
  pkt->option.ip.send.protocol = IP_PROTOCOL_UDP;
  pkt->option.ip.send.dst_addr = dst_ipaddr;
  kz_send(MSGBOX_ID_IPPROC, 0, (char *)pkt);

  return 0;
}

static struct netbuf *udp_makepkt(uint16 src_port, uint16 dst_port,
  int size, char *data)
{
  struct netbuf *pkt;
  struct udp_header *udphdr;
  int hdrlen = sizeof(struct udp_header);

  pkt = kz_kmalloc(DEFAULT_NETBUF_SIZE);
  memset(pkt, 0, DEFAULT_NETBUF_SIZE);
  pkt->top = pkt->data + 64;

  udphdr = (struct udp_header *)pkt->top;

  udphdr->src_port = src_port;
  udphdr->dst_port = dst_port;
  udphdr->length   = hdrlen + size;

  if (data)
    memcpy((char *)udphdr + hdrlen, data, size);

  pkt->size = hdrlen + size;

  return pkt;
}

static int udp_recv(struct netbuf *pkt)
{
  struct portinfo *pinfo;
  struct udp_header *udphdr;

  udphdr = (struct udp_header *)pkt->top;

  pinfo = udp_search_portinfo_from_addr(0,
          pkt->option.common.ipaddr.addr,
          udphdr->dst_port,
          udphdr->src_port);
  if (!pinfo)
    pinfo = udp_search_portinfo_from_addr(0, 0, udphdr->dst_port, 0);

  if (!pinfo) return 0;

  pkt->top  += sizeof(struct udp_header);
  pkt->size += sizeof(struct udp_header);

  /* データを上位タスクに通知 */
  pkt->cmd = UDP_CMD_RECV;
  pkt->option.udp.recv.ipaddr = pkt->option.common.ipaddr.addr;
  pkt->option.udp.recv.port = udphdr->src_port;
  kz_send(pinfo->id, 0, (char *)pkt);

  return 1;
}

static int udp_send(struct netbuf *buf)
{
  struct netbuf *pkt;

  pkt = udp_makepkt(buf->option.udp.send.src_port,
    buf->option.udp.send.dst_port, buf->size, buf->top);
  if (pkt)
    udp_sendpkt(pkt, my_ipaddr, buf->option.udp.send.ipaddr);

  return 0;
}

static int udp_proc(struct netbuf *buf)
{
  struct portinfo *pinfo;
  int ret = 0;

  switch (buf->cmd) {
  case UDP_CMD_IPADDR:
    my_ipaddr = buf->option.common.ipaddr.addr;
    break;

  case UDP_CMD_REGPORT:
    pinfo = kz_kmalloc(sizeof(*pinfo));
    memset(pinfo, 0, sizeof(*pinfo));

    pinfo->id         = buf->option.udp.regport.id;
    pinfo->src_ipaddr = my_ipaddr;
    pinfo->dst_ipaddr = buf->option.udp.regport.ipaddr;
    pinfo->src_port   = buf->option.udp.regport.src_port;
    pinfo->dst_port   = buf->option.udp.regport.dst_port;
    kz_send(MSGBOX_ID_UDPPORTLIST, 0, (char *)pinfo);
    udp_search_portinfo_from_addr(-1, -1, -1, -1); /* 頭出し */
    break;

  case UDP_CMD_RELPORT:
    pinfo = udp_delete_portinfo(buf->option.udp.relport.port);
    if (pinfo) udp_free_portinfo(pinfo);
    break;

  case UDP_CMD_IPRECV:
    ret = udp_recv(buf);
    break;

  case UDP_CMD_SEND:
    ret = udp_send(buf);
    break;

  default:
    break;
  }

  return ret;
}

int udp_main(int argc, char *argv[])
{
  struct netbuf *buf;
  int ret;

  kz_send(MSGBOX_ID_UDPPORTLIST, 0, NULL);

  buf = kz_kmalloc(sizeof(*buf));
  buf->cmd = IP_CMD_REGPROTO;
  buf->option.ip.regproto.protocol = IP_PROTOCOL_UDP;
  buf->option.ip.regproto.cmd      = UDP_CMD_IPRECV;
  buf->option.ip.regproto.id       = MSGBOX_ID_UDPPROC;
  kz_send(MSGBOX_ID_IPPROC, 0, (char *)buf);

  while (1) {
    kz_recv(MSGBOX_ID_UDPPROC, NULL, (char **)&buf);
    ret = udp_proc(buf);
    if (!ret) kz_kmfree(buf);
  }

  return 0;
}
