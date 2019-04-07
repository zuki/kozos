#include "defines.h"
#include "kozos.h"
#include "netdrv.h"
#include "ethernet.h"
#include "arp.h"
#include "ip.h"
#include "lib.h"

#define ETHERNET_HEADER_SIZE  14

struct ethernet_header {
  uint8 dst_addr[MACADDR_SIZE];
  uint8 src_addr[MACADDR_SIZE];
  uint16 type;
};

static unsigned char my_macaddr[MACADDR_SIZE];
static int initialized = 0;

static int ethernet_recv(struct netbuf *pkt)
{
  struct ethernet_header *ethhdr = (struct ethernet_header *)pkt->top;

  if (!initialized)
    return 0;

  if (!(ethhdr->dst_addr[0] & 1) &&
        memcmp(ethhdr->dst_addr, my_macaddr, MACADDR_SIZE))
    return 0;

  pkt->top  += ETHERNET_HEADER_SIZE;
  pkt->size -= ETHERNET_HEADER_SIZE;

  switch (ethhdr->type) {
    case ETHERNET_TYPE_ARP:
      pkt->cmd = ARP_CMD_RECV;
      kz_send(MSGBOX_ID_ARPPROC, 0, (char *)pkt);
      break;
    case ETHERNET_TYPE_IP:
      pkt->cmd = IP_CMD_RECV;
      kz_send(MSGBOX_ID_IPPROC, 0, (char *)pkt);
      break;
    default:
      return 0;
  }

  return 1;
}

static int ethernet_send(struct netbuf *pkt)
{
  struct ethernet_header *ethhdr;

  /* 送信先MACアドレスが不明なので、ARPタスクに転送して解決してもらう */
  if (!memcmp(pkt->option.ethernet.send.dst_macaddr,
        "\x00\x00\x00\x00\x00\x00", MACADDR_SIZE)) {
    pkt->cmd = ARP_CMD_SEND;
    kz_send(MSGBOX_ID_ARPPROC, 0, (char *)pkt);
    return 1;
  }

  pkt->cmd = NETDRV_CMD_SEND;

  pkt->top  -= ETHERNET_HEADER_SIZE;
  pkt->size += ETHERNET_HEADER_SIZE;
  ethhdr = (struct ethernet_header *)pkt->top;

  memcpy(ethhdr->dst_addr, pkt->option.ethernet.send.dst_macaddr, MACADDR_SIZE);
  memcpy(ethhdr->src_addr, my_macaddr, MACADDR_SIZE);
  ethhdr->type = pkt->option.ethernet.send.type;

  kz_send(MSGBOX_ID_NETPROC, 0, (char *)pkt);
  return 1;
}

static int ethernet_proc(struct netbuf *buf)
{
  int ret = 0;

  switch (buf->cmd) {
    case ETHERNET_CMD_MACADDR:
      memcpy(my_macaddr, buf->option.common.macaddr.addr, MACADDR_SIZE);
      buf->cmd = ARP_CMD_MACADDR;
      kz_send(MSGBOX_ID_ARPPROC, 0, (char *)buf);
      initialized++;
      ret = 1;
      break;
    case ETHERNET_CMD_RECV:
      ret = ethernet_recv(buf);
      break;
    case ETHERNET_CMD_SEND:
      ret = ethernet_send(buf);
      break;
    default:
      break;
  }

  return ret;
}

int ethernet_main(int argc, char *argv[])
{
  struct netbuf *buf;
  int ret;

  buf = kz_kmalloc(sizeof(*buf));
  buf->cmd = NETDRV_CMD_USE;
  kz_send(MSGBOX_ID_NETPROC, 0, (char *)buf);

  while (1) {
    kz_recv(MSGBOX_ID_ETHPROC, NULL, (char **)&buf);
    ret = ethernet_proc(buf);
    if (!ret) kz_kmfree(buf);
  }

  return 0;
}
