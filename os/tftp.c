#include "defines.h"
#include "kozos.h"
#include "netdrv.h"
#include "udp.h"
#include "tftp.h"
#include "lib.h"

union tftp_header {
  struct {
    uint16 opcode;
#define TFTP_OPCODE_READ   1
#define TFTP_OPCODE_WRITE  2
#define TFTP_OPCODE_DATA   3
#define TFTP_OPCODE_ACK    4
#define TFTP_OPCODE_ERR    5
#define TFTP_OPCODE_OPTACK 6
  } common;
  struct {
    uint16 opcode;
    char filename[1];
#define TFTP_REQUEST_MODE_ASCII  "netascii"
#define TFTP_REQUEST_MODE_BINARY "octet"
#define TFTP_REQUEST_MODE_MAIL   "mail"
  } request;
  struct {
    uint16 opcode;
    uint16 block_number;
    char data[1];
  } data;
  struct {
    uint16 opcode;
    uint16 block_number;
  } ack;
  struct {
    uint16 opcode;
    uint16 errcode;
    char message[1];
  } err;
  struct {
    uint16 opcode;
    char option[1];
  } optack;
};

static char tftp_buffer[1024*16]; /* テスト用に16KBのバッファを作成 */

static void send_regport(int port)
{
  struct netbuf *buf;
  buf = kz_kmalloc(sizeof(*buf));
  buf->cmd = UDP_CMD_REGPORT;
  buf->option.udp.regport.src_port = port;
  buf->option.udp.regport.dst_port = 0; /* 未指定 */
  buf->option.udp.regport.ipaddr   = 0; /* 未指定 */
  buf->option.udp.regport.id = MSGBOX_ID_TFTP;
  kz_send(MSGBOX_ID_UDPPROC, 0, (char *)buf);
}

static void send_relport(int port)
{
  struct netbuf *buf;
  buf = kz_kmalloc(sizeof(*buf));
  buf->cmd = UDP_CMD_RELPORT;
  buf->option.udp.relport.port = port;
  kz_send(MSGBOX_ID_UDPPROC, 0, (char *)buf);
}

static void send_read(int src_port, int dst_port, uint32 ipaddr,
		      char *filename)
{
  struct netbuf *buf;
  union tftp_header *tftphdr;
  char *p;

  buf = kz_kmalloc(DEFAULT_NETBUF_SIZE);
  memset(buf, 0, DEFAULT_NETBUF_SIZE);

  buf->cmd = UDP_CMD_SEND;
  buf->top = buf->data;
  buf->option.udp.send.src_port = src_port;
  buf->option.udp.send.dst_port = dst_port;
  buf->option.udp.send.ipaddr   = ipaddr;

  tftphdr = (union tftp_header *)buf->top;
  tftphdr->request.opcode = TFTP_OPCODE_READ;
  p = (char *)tftphdr->request.filename;
  strcpy(p, filename);
  p += strlen(p) + 1;
  strcpy(p, TFTP_REQUEST_MODE_BINARY);
  p += strlen(p) + 1;

  buf->size = p - (char *)buf->top;

  kz_send(MSGBOX_ID_UDPPROC, 0, (char *)buf);
}

static void send_ack(int src_port, int dst_port, uint32 ipaddr,
		     int block_number)
{
  struct netbuf *buf;
  union tftp_header *tftphdr;

  buf = kz_kmalloc(DEFAULT_NETBUF_SIZE);
  memset(buf, 0, DEFAULT_NETBUF_SIZE);

  buf->cmd = UDP_CMD_SEND;
  buf->top = buf->data;
  buf->size = 4;
  buf->option.udp.send.src_port = src_port;
  buf->option.udp.send.dst_port = dst_port;
  buf->option.udp.send.ipaddr   = ipaddr;

  tftphdr = (union tftp_header *)buf->top;
  tftphdr->ack.opcode = TFTP_OPCODE_ACK;
  tftphdr->ack.block_number = block_number;

  kz_send(MSGBOX_ID_UDPPROC, 0, (char *)buf);
}

static int tftp_recv(struct netbuf *pkt, uint32 ipaddr, uint16 port, char **p)
{
  union tftp_header *tftphdr;
  int size, dst_port, block_number;

  tftphdr = (union tftp_header *)pkt->top;

  switch (tftphdr->common.opcode) {
  case TFTP_OPCODE_DATA:
    size = pkt->size - ((char *)tftphdr->data.data - (char *)tftphdr);
    memcpy(*p, tftphdr->data.data, size);
    *p += size;
    dst_port = pkt->option.udp.recv.port;
    block_number = tftphdr->data.block_number;
    send_ack(port, dst_port, ipaddr, block_number);
    if (size < 512)
      return 1;
    break;
  case TFTP_OPCODE_OPTACK:
    dst_port = pkt->option.udp.recv.port;
    send_ack(port, dst_port, ipaddr, 0);
    break;
  }

  return 0;
}

int tftp_main(int argc, char *argv[])
{
  struct netbuf *buf;
  uint32 server = 0;
  int port = 60010, ret;
  char *buffer = NULL;

  while (1) {
    kz_recv(MSGBOX_ID_TFTP, NULL, (void *)&buf);
    switch (buf->cmd) {
    case TFTP_CMD_START:
      buffer = tftp_buffer;
      server = buf->option.tftp.start.ipaddr;
      ++port;
      send_regport(port);
      send_read(port, 69, server, "kozos");
      break;
    case UDP_CMD_RECV:
      ret = tftp_recv(buf, server, port, &buffer);
      if (ret == 1) {
	send_relport(port);
      }
      break;
    }
    kz_kmfree(buf);
  }

  return 0;
}
