#include "defines.h"
#include "kozos.h"
#include "netdrv.h"
#include "udp.h"
#include "lib.h"

#define ECHO_PORT   7

static void send_regport(int port)
{
  struct netbuf *buf;

  buf = kz_kmalloc(sizeof(*buf));
  buf->cmd = UDP_CMD_REGPORT;
  buf->option.udp.regport.src_port = port;
  buf->option.udp.regport.dst_port = 0;
  buf->option.udp.regport.ipaddr   = 0;
  buf->option.udp.regport.id = MSGBOX_ID_ECHO;
  kz_send(MSGBOX_ID_UDPPROC, 0, (char *)buf);
}

static void echo_proc(struct netbuf *pkt)
{
  struct netbuf *buf;

  buf = kz_kmalloc(DEFAULT_NETBUF_SIZE);
  memset(buf, 0, DEFAULT_NETBUF_SIZE);

  buf->cmd = UDP_CMD_SEND;
  buf->top = buf->data + 64;
  buf->size = strlen(pkt->top);
  memcpy(buf->top, pkt->top, buf->size);
  buf->option.udp.send.src_port = ECHO_PORT;
  buf->option.udp.send.dst_port = pkt->option.udp.recv.port;
  buf->option.udp.send.ipaddr   = pkt->option.udp.recv.ipaddr;

  kz_send(MSGBOX_ID_UDPPROC, 0, (char *)buf);
}

int echo_main(int argc, char *argv[])
{
  struct netbuf *buf;

  send_regport(ECHO_PORT);

  while (1) {
    kz_recv(MSGBOX_ID_ECHO, NULL, (char **)&buf);
    switch (buf->cmd) {
    case UDP_CMD_RECV:
      echo_proc(buf);
      break;
    default:
      break;
    }
    kz_kmfree(buf);
  }

  return 0;
}
