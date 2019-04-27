#include "defines.h"
#include "kozos.h"
#include "netdrv.h"
#include "ethernet.h"
#include "arp.h"
#include "lib.h"

struct arp_header {
  uint16 hardware;
#define ARP_HARDWARE_ETHER  1
  uint16 protocol;
  uint8  macaddr_size;
  uint8  ipaddr_size;
  uint16 operation;
#define ARP_OPERATION_REQUEST 1
#define ARP_OPERATION_REPLY   2

  uint8  sender_macaddr[MACADDR_SIZE];
  uint8  sender_ipaddr[IPADDR_SIZE];
  uint8  target_macaddr[MACADDR_SIZE];
  uint8  target_ipaddr[IPADDR_SIZE];
};

struct addrset {
  uint32  ipaddr;
  uint8   macaddr[MACADDR_SIZE];
};

static unsigned char my_macaddr[MACADDR_SIZE];
static uint32   my_ipaddr;

static struct addrset *arp_getaddr(uint32 ipaddr)
{
  struct addrset *addr, *ret = NULL;

  while (1) {
    kz_recv(MSGBOX_ID_ARPADRLIST, NULL, (char **)&addr);
    if (addr == NULL) { /* 終端まできた */
      kz_send(MSGBOX_ID_ARPADRLIST, 0, NULL);
      break;
    }
    if (addr->ipaddr == ipaddr) {
      ret = addr;
    }
    kz_send(MSGBOX_ID_ARPADRLIST, 0, (char *)addr);
  }
  return ret;
}

static struct addrset *arp_setaddr(uint32 ipaddr, uint8 macaddr[])
{
  struct addrset *addr;

  addr = arp_getaddr(ipaddr);
  if (addr == NULL) {
    addr = kz_kmalloc(sizeof(*addr));
    memset(addr, 0, sizeof(*addr));
    addr->ipaddr = ipaddr;
    kz_send(MSGBOX_ID_ARPADRLIST, 0, (char *)addr);
    /*
     * 終端のターミネータのさらに後ろに追加したので、もう一周させて
     * ターミネータが終端にくるようにする
     */
    arp_getaddr(ipaddr);
  }
  memcpy(addr->macaddr, macaddr, MACADDR_SIZE);
  return addr;
}

static void arp_flush(int count)
{
  struct netbuf  *pkt;
  struct addrset *addr;

  while (1) {
    kz_recv(MSGBOX_ID_ARPPKTLIST, NULL, (char **)&pkt);
    if (pkt == NULL) {
      kz_send(MSGBOX_ID_ARPPKTLIST, 0, NULL);
      break;
    }

    addr = arp_getaddr(pkt->option.ethernet.send.dst_ipaddr);
    if (addr) {
      addr = (count == 0) ? NULL : addr;
      count = (count > 0) ? (count - 1) : count;
    }

    if (addr == NULL) {
      kz_send(MSGBOX_ID_ARPPKTLIST, 0, (char *)pkt);
      continue;
    }

    memcpy(pkt->option.ethernet.send.dst_macaddr, addr->macaddr, MACADDR_SIZE);

    pkt->cmd = ETHERNET_CMD_SEND;
    kz_send(MSGBOX_ID_ETHPROC, 0, (char *)pkt);
  }
}

static int arp_sendpkt(uint16 operation, uint8 macaddr[], uint32 ipaddr)
{
  struct netbuf     *pkt;
  struct arp_header *arphdr;

  pkt = kz_kmalloc(DEFAULT_NETBUF_SIZE);
  memset(pkt, 0, DEFAULT_NETBUF_SIZE);
  pkt->cmd = ETHERNET_CMD_SEND;
  pkt->top = pkt->data + 64;
  pkt->size = sizeof(struct arp_header);

  arphdr = (struct arp_header *)pkt->top;

  arphdr->hardware     = ARP_HARDWARE_ETHER;
  arphdr->protocol     = ETHERNET_TYPE_IP;
  arphdr->macaddr_size = MACADDR_SIZE;
  arphdr->ipaddr_size  = IPADDR_SIZE;
  arphdr->operation    = operation;

  memcpy(arphdr->sender_macaddr, my_macaddr, MACADDR_SIZE);
  memcpy(arphdr->sender_ipaddr, &my_ipaddr, IPADDR_SIZE);
  memcpy(arphdr->target_macaddr,
    (macaddr ? macaddr: (uint8 *)"\x00\x00\x00\x00\x00\x00"),
    MACADDR_SIZE);
  memcpy(arphdr->target_ipaddr, &ipaddr, IPADDR_SIZE);

  memcpy(pkt->option.ethernet.send.dst_macaddr,
    (macaddr ? macaddr : (uint8 *)"\xff\xff\xff\xff\xff\xff"),
    MACADDR_SIZE);
  pkt->option.ethernet.send.type = ETHERNET_TYPE_ARP;

  kz_send(MSGBOX_ID_ETHPROC, 0, (char *)pkt);

  return 0;
}

static int arp_recv(struct netbuf *pkt)
{
  struct addrset    *addr;
  struct arp_header *arphdr;
  uint32            ipaddr;

  arphdr = (struct arp_header *)pkt->top;

  if (arphdr->hardware != ARP_HARDWARE_ETHER)
    return 0;
  if (arphdr->protocol != ETHERNET_TYPE_IP)
    return 0;

  switch (arphdr->operation) {
    case ARP_OPERATION_REPLY:
    case ARP_OPERATION_REQUEST:
      memcpy(&ipaddr, arphdr->sender_ipaddr, IPADDR_SIZE);
      addr = arp_setaddr(ipaddr, arphdr->sender_macaddr);
      arp_flush(-1);
      if (arphdr->operation == ARP_OPERATION_REPLY)
        break;

      if (memcmp(arphdr->target_ipaddr, &my_ipaddr, IPADDR_SIZE))
        break;

      /* ARP reply を転送 */
      arp_sendpkt(ARP_OPERATION_REPLY, addr->macaddr, addr->ipaddr);
      break;
    default:
      break;
  }

  return 0;
}

static int arp_send(struct netbuf *pkt)
{
  struct addrset *addr;

  kz_send(MSGBOX_ID_ARPPKTLIST, 0, (char *)pkt);
  arp_flush(0); /* 終端に追加したので1周させ頭出しを行う */

  addr = arp_getaddr(pkt->option.ethernet.send.dst_ipaddr);

  if (addr) {
    arp_flush(-1);
  } else {
    /* ARP request を送信 */
    arp_sendpkt(ARP_OPERATION_REQUEST, NULL,
                pkt->option.ethernet.send.dst_ipaddr);
  }

  return 1;
}

static int arp_proc(struct netbuf *buf)
{
  int ret = 0;

  switch (buf->cmd) {
    case ARP_CMD_MACADDR:
      memcpy(my_macaddr, buf->option.common.macaddr.addr, MACADDR_SIZE);
      break;
    case ARP_CMD_IPADDR:
      my_ipaddr = buf->option.common.ipaddr.addr;
      break;
    case ARP_CMD_RECV:
      ret = arp_recv(buf);
      break;
    case ARP_CMD_SEND:
      ret = arp_send(buf);
      break;
    default:
      break;
  }

  return ret;
}

int arp_main(int argc, char *argv[])
{
  struct netbuf *buf;
  int ret;

  kz_send(MSGBOX_ID_ARPADRLIST, 0, NULL);
  kz_send(MSGBOX_ID_ARPPKTLIST, 0, NULL);

  while (1) {
    kz_recv(MSGBOX_ID_ARPPROC, NULL, (char **)&buf);
    ret = arp_proc(buf);
    if (!ret) kz_kmfree(buf);
  }

  return 0;
}
