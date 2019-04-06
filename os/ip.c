#include "defines.h"
#include "kozos.h"
#include "ether.h"
#include "etherdrv.h"
#include "lib.h"

uint32 network_id;

#define MACADDR "\x00\x11\x22\x33\x44\x55"
#define IPADDR  0xc0a80b0b  /* 192.168.11.11 */
static uint32 ipaddr = IPADDR;

#define MACADDR_SIZE  6
#define IPADDR_SIZE   4

#define ETHERNET_HEADER_SIZE  14

struct ethernet_header {
  uint8 dst_addr[MACADDR_SIZE];
  uint8 src_addr[MACADDR_SIZE];
  uint16 type;
#define ETHERNET_TYPE_IP  0x0800
#define ETHERNET_TYPE_ARP 0x0806
};

struct arp_header {
  uint16 hardware;
#define ARP_HARDWARE_ETHER  1
  uint16 protocol;
  uint8  hardware_addr_size;
  uint8  protocol_addr_size;
  uint16 operation;
#define ARP_OPERATION_REQUEST 1
#define ARP_OPERATION_REPLY   2

  uint8  sender_hardware_addr[MACADDR_SIZE];
  uint8  sender_protocol_addr[IPADDR_SIZE];
  uint8  target_hardware_addr[MACADDR_SIZE];
  uint8  target_protocol_addr[IPADDR_SIZE];
};

struct ip_header {
  uint8  v_hl;
  uint8  tos;
  uint16 total_length;
  uint16 id;
  uint16 fragment;
  uint8  ttl;
  uint8  protocol;
#define IP_PROTOCOL_ICMP  1
#define IP_PROTOCOL_TCP   6
#define IP_PROTOCOL_UDP  17

  uint16 checksum;
  uint8  src_addr[IPADDR_SIZE];
  uint8  dst_addr[IPADDR_SIZE];
};

struct icmp_header {
  uint8  type;
#define ICMP_TYPE_REPLY   0
#define ICMP_TYPE_REQUEST 8

  uint8  code;
  uint16 checksum;
};

static uint16 calc_checksum(int size, void *buf)
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

static int icmp_proc(int size, struct icmp_header *hdr)
{
  int ret = 0;

  switch (hdr->type) {
    case ICMP_TYPE_REQUEST:
      hdr->type = ICMP_TYPE_REPLY;
      hdr->checksum = 0;
      hdr->checksum = calc_checksum(size, hdr);
      ret = size;
      break;
    default:
      break;
  }

  return ret;
}

static int ip_proc(int size, struct ip_header *hdr)
{
  int ret = 0, hdrlen, nextsize;
  void *nexthdr;

  if (((hdr->v_hl >> 4) & 0xf) != 4)
    return 0;
  if (memcmp(hdr->dst_addr, &ipaddr, IPADDR_SIZE))
    return 0;

  hdrlen = (hdr->v_hl & 0xf) << 2;

  if (size > hdr->total_length)
    size = hdr->total_length;

  nextsize = size - hdrlen;
  nexthdr  = (char *)hdr + hdrlen;

  switch (hdr->protocol) {
    case IP_PROTOCOL_ICMP:
      ret = icmp_proc(nextsize, nexthdr);
      break;
    default:
      break;
  }

  if (ret > 0) {
    memcpy(hdr->dst_addr, hdr->src_addr, IPADDR_SIZE);
    memcpy(hdr->src_addr, &ipaddr, IPADDR_SIZE);
    hdr->checksum = 0;
    hdr->checksum = calc_checksum(hdrlen, hdr);
    ret += hdrlen;
  }

  return ret;
}

static int arp_proc(int size, struct arp_header *hdr)
{
  int ret = 0;

  if (hdr->hardware != ARP_HARDWARE_ETHER)
    return 0;
  if (hdr->protocol != ETHERNET_TYPE_IP)
    return 0;

  switch (hdr->operation) {
    case ARP_OPERATION_REQUEST:
      if (memcmp(hdr->target_protocol_addr, &ipaddr, IPADDR_SIZE))
        break;
      memcpy(hdr->target_hardware_addr, hdr->sender_hardware_addr, MACADDR_SIZE);
      memcpy(hdr->target_protocol_addr, hdr->sender_protocol_addr, IPADDR_SIZE);
      memcpy(hdr->sender_hardware_addr, MACADDR, MACADDR_SIZE);
      memcpy(hdr->sender_protocol_addr, &ipaddr, IPADDR_SIZE);
      hdr->operation = ARP_OPERATION_REPLY;
      ret = size;
      break;
    default:
      break;
  }

  return ret;
}

static int ethernet_proc(int size, struct ethernet_header *hdr)
{
  int ret = 0, nextsize;
  void *nexthdr;

  if (!(hdr->dst_addr[0] & 1) && memcmp(hdr->dst_addr, MACADDR, MACADDR_SIZE))
    return 0;

  nextsize = size - ETHERNET_HEADER_SIZE;
  nexthdr = (char *)hdr + ETHERNET_HEADER_SIZE;

  switch (hdr->type) {
    case ETHERNET_TYPE_ARP:
      ret = arp_proc(nextsize, nexthdr);
      break;
    case ETHERNET_TYPE_IP:
      ret = ip_proc(nextsize, nexthdr);
      break;
    default:
      break;
  }

  if (ret > 0) {
    memcpy(hdr->dst_addr, hdr->src_addr, MACADDR_SIZE);
    memcpy(hdr->src_addr, MACADDR, MACADDR_SIZE);
    ret += ETHERNET_HEADER_SIZE;
  }

  return ret;
}

static void send_use(void)
{
  char *p;

  p = kz_kmalloc(1);
  p[0] = ETHERDRV_CMD_USE;
  kz_send(MSGBOX_ID_ETHOUTPUT, 1, p);
}

static void send_send(int size, char *buf)
{
  char *p;

  p = kz_kmalloc(size + 1);
  p[0] = ETHERDRV_CMD_SEND;
  memcpy(&p[1], buf, size);
  kz_send(MSGBOX_ID_ETHOUTPUT, size + 1, p);
}

int ip_main(int argc, char *argv[])
{
  char *p;
  int size;

  send_use();
  puts("network ready.\n");

  while (1) {
    kz_recv(MSGBOX_ID_ETHINPUT, &size, &p);
    size = ethernet_proc(size, (struct ethernet_header *)p);
    puts("received: 0x");
    putxval(size, 0);
    puts(" bytes\n");
    if (size > 0) {
      send_send(size, p);
      puts("replyed.\n");
    } else {
      puts("no reply.\n");
    }
    kz_kmfree(p);
  }

  return 0;
}
