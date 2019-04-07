#include "defines.h"
#include "kozos.h"
#include "netdrv.h"
#include "ip.h"
#include "icmp.h"
#include "lib.h"

struct icmp_header {
  uint8  type;
#define ICMP_TYPE_REPLY   0
#define ICMP_TYPE_REQUEST 8

  uint8  code;
  uint16 checksum;
  struct {
    uint16  id;
    uint16  sequence_number;
  } param;
};

static int icmp_sendpkt(uint32 ipaddr, uint8 type, uint8 code, uint16 id,
                  uint16 sequence_number, int datasize, char *data)
{
  struct netbuf *pkt;
  struct icmp_header *icmphdr;
  int i;
  char c;

  pkt = kz_kmalloc(DEFAULT_NETBUF_SIZE);
  memset(pkt, 0, DEFAULT_NETBUF_SIZE);

  pkt->cmd  = IP_CMD_SEND;
  pkt->top  = pkt->data + 64;
  pkt->size = sizeof(struct icmp_header) + datasize;

  icmphdr = (struct icmp_header *)pkt->top;
  icmphdr->type = type;
  icmphdr->code = code;
  icmphdr->param.id = id;
  icmphdr->param.sequence_number = sequence_number;

  for (i = 0; i < datasize; i++) {
    c = data ? data[i] : (i & 0xff);
    ((char *)icmphdr + sizeof(*icmphdr))[i] = c;
  }

  icmphdr->checksum = 0;
  icmphdr->checksum = ip_calc_checksum(pkt->size, icmphdr);

  pkt->option.ip.send.protocol = IP_PROTOCOL_ICMP;
  pkt->option.ip.send.dst_addr = ipaddr;

  kz_send(MSGBOX_ID_IPPROC, 0, (char *)pkt);

  return 0;
}

static int icmp_recv(struct netbuf *pkt)
{
  struct icmp_header *icmphdr;

  icmphdr = (struct icmp_header *)pkt->top;

  /* 正式にはコンソールドライバタスクに依頼する必要がある */
  puts("ICMP received: ");
  putxval(pkt->option.common.ipaddr.addr, 8); puts(" ");
  putxval(icmphdr->type, 2); puts(" ");
  putxval(icmphdr->code, 2); puts(" ");
  putxval(icmphdr->checksum, 4); puts("\n");

  switch (icmphdr->type) {
    case ICMP_TYPE_REPLY:
      break;
    case ICMP_TYPE_REQUEST:
      icmp_sendpkt(pkt->option.common.ipaddr.addr,
        ICMP_TYPE_REPLY, icmphdr->code,
        icmphdr->param.id, icmphdr->param.sequence_number,
        pkt->size - sizeof(struct icmp_header),
        (char *)icmphdr + sizeof(*icmphdr));
      break;
  }

  return 0;
}

static int icmp_send(struct netbuf *pkt)
{
  static int id = 0;
  int i;

  id++;
  for (i = 0; i < pkt->option.icmp.send.number; i++) {
    icmp_sendpkt(pkt->option.icmp.send.ipaddr,
      ICMP_TYPE_REQUEST, 0, id, i,
      64 - sizeof(struct icmp_header), NULL);
  }

  return 0;
}

static int icmp_proc(struct netbuf *buf)
{
  int ret = 0;

  switch (buf->cmd) {
    case ICMP_CMD_RECV:
      ret = icmp_recv(buf);
      break;
    case ICMP_CMD_SEND:
      ret = icmp_send(buf);
      break;
    default:
      break;
  }

  return ret;
}

int icmp_main(int argc, char *argv[])
{
  struct netbuf *buf;
  int ret;

  buf = kz_kmalloc(sizeof(*buf));
  buf->cmd = IP_CMD_REGPROTO;
  buf->option.ip.regproto.protocol = IP_PROTOCOL_ICMP;
  buf->option.ip.regproto.cmd      = ICMP_CMD_RECV;
  buf->option.ip.regproto.id       = MSGBOX_ID_ICMPPROC;
  kz_send(MSGBOX_ID_IPPROC, 0, (char *)buf);

  while (1) {
    kz_recv(MSGBOX_ID_ICMPPROC, NULL, (char **)&buf);
    ret = icmp_proc(buf);
    if (!ret) kz_kmfree(buf);
  }

  return 0;
}
