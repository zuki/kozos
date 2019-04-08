#include "kozos.h"
#include "netdrv.h"
#include "ip.h"
#include "tcp.h"
#include "lib.h"

struct tcp_header {
  uint16 src_port;
  uint16 dst_port;
  uint32 seq_number;
  uint32 ack_number;
  uint8 offset;
  uint8 flags;
#define TCP_HEADER_FLAG_FIN (1 << 0)
#define TCP_HEADER_FLAG_SYN (1 << 1)
#define TCP_HEADER_FLAG_RST (1 << 2)
#define TCP_HEADER_FLAG_PSH (1 << 3)
#define TCP_HEADER_FLAG_ACK (1 << 4)
#define TCP_HEADER_FLAG_URG (1 << 5)
#define TCP_HEADER_FLAG_ECE (1 << 6)
#define TCP_HEADER_FLAG_CWR (1 << 7)
#define TCP_HEADER_FLAG_SYNACK (TCP_HEADER_FLAG_SYN | TCP_HEADER_FLAG_ACK)
#define TCP_HEADER_FLAG_FINACK (TCP_HEADER_FLAG_FIN | TCP_HEADER_FLAG_ACK)
  uint16 window;
  uint16 checksum;
  uint16 urgent;
};

#define TCP_OPTION_TYPE_NOP       1
#define TCP_OPTION_TYPE_MAXSEG    2
#define TCP_OPTION_TYPE_WINDOW    3
#define TCP_OPTION_LENGTH_WINDOW  3
#define TCP_OPTION_LENGTH_MAXSEG  4

struct connection {
  int number;
  kz_msgbox_id_t id;
  uint32 src_ipaddr;
  uint32 dst_ipaddr;
  uint16 src_port;
  uint16 dst_port;
  uint32 snd_number;
  uint32 seq_number;
  uint32 ack_number;
  enum {
    TCP_CONNECTION_STATUS_CLOSED,
    TCP_CONNECTION_STATUS_LISTEN,
    TCP_CONNECTION_STATUS_SYNSENT,
    TCP_CONNECTION_STATUS_SYNRECV,
    TCP_CONNECTION_STATUS_ESTAB,
    TCP_CONNECTION_STATUS_FINWAIT1,
    TCP_CONNECTION_STATUS_FINWAIT2,
    TCP_CONNECTION_STATUS_CLOSEWAIT,
    TCP_CONNECTION_STATUS_LASTACK,
  } status;
  struct netbuf *send_queue;
  struct netbuf **send_queue_end;
  struct netbuf *recv_queue;
  struct netbuf **recv_queue_end;
};

static uint32 my_ipaddr;
static uint16 port = 60000;
static int number = 0;

static struct connection *tcp_search_connection_from_addr(
  uint32 src_ipaddr, uint32 dst_ipaddr,
  uint16 src_port,   uint16 dst_port)
{
  struct connection *con, *ret = NULL;

  while (1) {
    kz_recv(MSGBOX_ID_TCPCONLIST, NULL, (char **)&con);
    if (con == NULL) {  /* 終端まできた */
      kz_send(MSGBOX_ID_TCPCONLIST, 0, NULL);
      break;
    }
    if (!src_ipaddr || (con->src_ipaddr == src_ipaddr)) {
      if (!dst_ipaddr || (con->dst_ipaddr == dst_ipaddr))
        if (!src_port || (con->src_port == src_port))
          if (!dst_port || (con->dst_port == dst_port))
            ret = con;
    }
    kz_send(MSGBOX_ID_TCPCONLIST, 0, (char *)con);
  }
  return ret;
}

static struct connection *tcp_search_connection_from_number(int number)
{
  struct connection *con, *ret = NULL;

  while (1) {
    kz_recv(MSGBOX_ID_TCPCONLIST, NULL, (char **)&con);
    if (con == NULL) {
      kz_send(MSGBOX_ID_TCPCONLIST, 0, NULL);
      break;
    }
    if (con->number == number) {
      ret = con;
    }
    kz_send(MSGBOX_ID_TCPCONLIST, 0, (char *)con);
  }
  return ret;
}

static struct connection *tcp_delete_connection(int number)
{
  struct connection *con, *ret = NULL;

  while (1) {
    kz_recv(MSGBOX_ID_TCPCONLIST, NULL, (char **)&con);
    if (con == NULL) {
      kz_send(MSGBOX_ID_TCPCONLIST, 0, NULL);
      break;
    }
    if (con->number == number) {
      ret = con;
    } else {
      kz_send(MSGBOX_ID_TCPCONLIST, 0, (char *)con);
    }
  }
  return ret;
}

static uint16 tcp_calc_pseudosum(uint32 src_ipaddr, uint32 dst_ipaddr,
  uint8 protocol, uint16 size)
{
  struct pseudo_header { /* TCPチェックサム計算用疑似ヘッダ */
    uint32 src_ipaddr;
    uint32 dst_ipaddr;
    uint8 zero;
    uint8 protocol;
    uint16 size;
  } phdr;

  phdr.src_ipaddr = src_ipaddr;
  phdr.dst_ipaddr = dst_ipaddr;
  phdr.zero       = 0;
  phdr.protocol   = protocol;
  phdr.size       = size;

  return ~ip_calc_checksum(sizeof(phdr), &phdr);
}

static int tcp_sendpkt(struct netbuf *pkt, struct connection *con)
{
  int hdrlen, size;
  struct tcp_header *tcphdr;

  tcphdr = (struct tcp_header *)pkt->top;
  hdrlen = ((tcphdr->offset >> 4) & 0xf) << 2;

  tcphdr->seq_number = con->seq_number;
  tcphdr->ack_number = con->ack_number;

  size = 0;
  if (tcphdr->flags & TCP_HEADER_FLAG_PSH) {
    size = pkt->size - hdrlen;
  } else if (tcphdr->flags &
    (TCP_HEADER_FLAG_SYN|TCP_HEADER_FLAG_FIN)) {
      size = 1;
  }

  tcphdr->checksum = tcp_calc_pseudosum(con->src_ipaddr,
    con->dst_ipaddr, IP_PROTOCOL_TCP, pkt->size);
  tcphdr->checksum = ip_calc_checksum(pkt->size, tcphdr);

  pkt->cmd = IP_CMD_SEND;
  pkt->option.ip.send.protocol = IP_PROTOCOL_TCP;
  pkt->option.ip.send.dst_addr = con->dst_ipaddr;
  kz_send(MSGBOX_ID_IPPROC, 0, (char *)pkt);

  con->snd_number = con->seq_number + size;

  return 0;
}

static struct netbuf *tcp_makepkt(struct connection *con,
  uint8 flags, uint16 window, int mss, int window_scale,
  int size, char *data)
{
  struct netbuf *pkt;
  struct tcp_header *tcphdr;
  int hdrlen = sizeof(struct tcp_header);
  char *option;

  pkt = kz_kmalloc(DEFAULT_NETBUF_SIZE);
  memset(pkt, 0, DEFAULT_NETBUF_SIZE);
  pkt->top = pkt->data + 64;

  tcphdr = (struct tcp_header *)pkt->top;

  tcphdr->src_port = con->src_port;
  tcphdr->dst_port = con->dst_port;
  tcphdr->flags    = flags;
  tcphdr->window   = window;
  tcphdr->urgent   = 0;

  option = (char *)(tcphdr + 1);
  if (mss) {
    *option = TCP_OPTION_TYPE_MAXSEG;   option++;
    *option = TCP_OPTION_LENGTH_MAXSEG; option++;
    *(uint16 *)option = mss;  option++; option++;
  }
  if (window_scale) {
    *option = TCP_OPTION_TYPE_NOP;      option++;
    *option = TCP_OPTION_TYPE_WINDOW;   option++;
    *option = TCP_OPTION_LENGTH_WINDOW; option++;
    *option = window_scale;             option++;
  }
  hdrlen += option - (char *)(tcphdr + 1);

  if (data)
    memcpy((char *)tcphdr + hdrlen, data, size);

  tcphdr->offset = ((hdrlen >> 2) & 0xf) << 4;
  pkt->size = hdrlen + size;

  return pkt;
}

static int tcp_makesendpkt(struct connection *con,
  uint8 flags, uint16 window, int mss, int window_scale,
  int size, char *data)
{
  struct netbuf *pkt;

  pkt = tcp_makepkt(con, flags, window, mss, window_scale, size, data);
  if (pkt)
    return tcp_sendpkt(pkt, con);
  return 0;
}

static int tcp_send_flush(struct connection *con)
{
  struct netbuf *pkt;

  if (con->snd_number == con->seq_number) {
    pkt = con->send_queue;
    if (pkt) {
      con->send_queue = pkt->next;
      if (!con->send_queue)
        con->send_queue_end = &(con->send_queue);
      pkt->next = NULL;
      tcp_sendpkt(pkt, con);
    }
  }
  return 0;
}

static int tcp_send_enqueue(struct connection *con,
  uint8 flags, uint16 window, int mss, int window_scale,
  int size, char *data)
{
  struct netbuf *pkt;

  pkt = tcp_makepkt(con, flags, window, mss, window_scale, size, data);

  pkt->next = NULL;
  *(con->send_queue_end) = pkt;
  con->send_queue_end = &(pkt->next);

  tcp_send_flush(con);

  return 0;
}

static int tcp_recv_open(struct netbuf *pkt,
  struct connection *con, struct tcp_header *tcphdr)
{
  struct netbuf *buf;

  switch (tcphdr->flags & TCP_HEADER_FLAG_SYNACK) {
    case TCP_HEADER_FLAG_SYN:
      if (con->status != TCP_CONNECTION_STATUS_LISTEN) break;

      con->snd_number = con->seq_number = 1;
      con->dst_ipaddr = pkt->option.common.ipaddr.addr;
      con->dst_port   = tcphdr->src_port;
      con->ack_number = tcphdr->seq_number + 1;

      tcp_makesendpkt(con, TCP_HEADER_FLAG_SYNACK, 1460, 1460, 1, 0, NULL);
      con->status = TCP_CONNECTION_STATUS_SYNRECV;
      break;

    case TCP_HEADER_FLAG_SYNACK:
      if (con->status != TCP_CONNECTION_STATUS_SYNSENT) break;

      con->seq_number = tcphdr->ack_number;
      con->ack_number = tcphdr->seq_number + 1;

      tcp_makesendpkt(con, TCP_HEADER_FLAG_ACK, 1460, 0, 0, 0, NULL);

      buf = kz_kmalloc(sizeof(*buf));
      buf->cmd = TCP_CMD_ESTAB;
      buf->option.tcp.establish.number = con->number;
      kz_send(con->id, 0, (char *)buf);

      con->status = TCP_CONNECTION_STATUS_ESTAB;
      break;

    case TCP_HEADER_FLAG_ACK:
      if (con->status != TCP_CONNECTION_STATUS_SYNRECV) break;

      con->seq_number = tcphdr->ack_number;

      buf = kz_kmalloc(sizeof(*buf));
      buf->cmd = TCP_CMD_ESTAB;
      buf->option.tcp.establish.number = con->number;
      kz_send(con->id, 0, (char *)buf);

      con->status = TCP_CONNECTION_STATUS_ESTAB;
      break;

    default:
      break;
  }

  return 0;
}

static int tcp_recv_close(struct netbuf *pkt,
  struct connection *con, struct tcp_header *tcphdr)
{
  struct netbuf *buf;
  int closed = 0;

  if ((tcphdr->flags & TCP_HEADER_FLAG_FINACK) == TCP_HEADER_FLAG_ACK) {
    switch (con->status) {
      case TCP_CONNECTION_STATUS_FINWAIT1:
        con->seq_number = tcphdr->ack_number;
        con->status = TCP_CONNECTION_STATUS_FINWAIT2;
        break;

      case TCP_CONNECTION_STATUS_LASTACK:
      default: /* ACKがすれ違った場合 */
        con->seq_number = tcphdr->ack_number;
        con->status = TCP_CONNECTION_STATUS_CLOSED;
        closed++;
        break;
    }
  }

  if ((tcphdr->flags & TCP_HEADER_FLAG_FINACK) == TCP_HEADER_FLAG_FINACK) {
    switch (con->status) {
      case TCP_CONNECTION_STATUS_FINWAIT2:
        con->ack_number = tcphdr->seq_number + 1;
        tcp_makesendpkt(con, TCP_HEADER_FLAG_ACK, 1460, 0, 0, 0, NULL);
        con->status = TCP_CONNECTION_STATUS_CLOSED;
        closed++;
        break;

      case TCP_CONNECTION_STATUS_ESTAB:
      default:  /* FINがすれ違った場合 */
        con->ack_number = tcphdr->seq_number + 1;
        tcp_makesendpkt(con, TCP_HEADER_FLAG_ACK, 1460, 0, 0, 0, NULL);
        con->status = TCP_CONNECTION_STATUS_CLOSEWAIT;
        tcp_makesendpkt(con, TCP_HEADER_FLAG_FINACK, 1460, 0, 0, 0, NULL);
        con->status = TCP_CONNECTION_STATUS_LASTACK;
        break;
    }
  }

  if (closed) {
    buf = kz_kmalloc(sizeof(*buf));
    buf->cmd = TCP_CMD_CLOSE;
    buf->option.tcp.close.number = con->number;
    kz_send(con->id, 0, (char *)buf);

    con = tcp_delete_connection(con->number);
    kz_kmfree(con);

    return 1;
  }

  return 0;
}

static int tcp_recv_flush(struct connection *con)
{
  struct netbuf *pkt, *next, **prevp;
  struct tcp_header *tcphdr;
  int hdrlen;

  prevp = &(con->recv_queue);

  for (pkt = con->recv_queue; pkt; pkt = next) {
    tcphdr = (struct tcp_header *)pkt->top;

    next = pkt->next;
    if (tcphdr->seq_number != con->ack_number) {
      prevp = &(pkt->next);
      continue;
    }

    *prevp = next;
    if (!*prevp)
      con->recv_queue_end = prevp;
    pkt->next = NULL;

    /* ACKを返す */
    hdrlen = ((tcphdr->offset >> 4) & 0xf) << 2;
    pkt->top  += hdrlen;
    pkt->size -= hdrlen;
    con->ack_number = tcphdr->seq_number + pkt->size;
    tcp_makesendpkt(con, TCP_HEADER_FLAG_ACK, 1460, 0, 0, 0, NULL);

    pkt->cmd = TCP_CMD_RECV;
    kz_send(con->id, 0, (char *)pkt);
  }

  return 0;
}

static int tcp_recv_data(struct netbuf *pkt,
  struct connection *con, struct tcp_header *tcphdr)
{
  /* ACKを受信 */
  if (tcphdr->flags & TCP_HEADER_FLAG_ACK) {
    /*
     * 本来ならここで保持しているデータを破棄し、一定時間
     * ACKがこなければ再送処理を行うべき
     */
    con->seq_number = tcphdr->ack_number;
    tcp_send_flush(con);
  }

  /* データを受信 */
  if (tcphdr->flags & TCP_HEADER_FLAG_PSH) {
    pkt->next = NULL;
    *(con->recv_queue_end) = pkt;
    con->recv_queue_end = &(pkt->next);
    tcp_recv_flush(con);
    return 1;
  }

  return 0;
}

static int tcp_recv(struct netbuf *pkt)
{
  struct netbuf *buf;
  struct connection *con;
  struct tcp_header *tcphdr;

  tcphdr = (struct tcp_header *)pkt->top;

  con = tcp_search_connection_from_addr(0,
    pkt->option.common.ipaddr.addr,
    tcphdr->dst_port, tcphdr->src_port);
  if (!con)
    con = tcp_search_connection_from_addr(0, 0, tcphdr->dst_port, 0);

  if (!con) return 0;

  if (tcphdr->flags & TCP_HEADER_FLAG_RST) {
    buf = kz_kmalloc(sizeof(*buf));
    buf->cmd = TCP_CMD_CLOSE;
    buf->option.tcp.close.number = con->number;
    kz_send(con->id, 0, (char *)buf);

    con = tcp_delete_connection(con->number);
    kz_kmfree(con);
    return 0;
  }

  switch (con->status) {
    case TCP_CONNECTION_STATUS_LISTEN:
    case TCP_CONNECTION_STATUS_SYNSENT:
    case TCP_CONNECTION_STATUS_SYNRECV:
      return tcp_recv_open(pkt, con, tcphdr);

    case TCP_CONNECTION_STATUS_ESTAB:
      if (tcphdr->flags & TCP_HEADER_FLAG_FIN)
        if (tcp_recv_close(pkt, con, tcphdr))
          return 1; /* conが削除されたので処理継続できないので返る。要修正 */
      return tcp_recv_data(pkt, con, tcphdr);

    case TCP_CONNECTION_STATUS_FINWAIT1:
    case TCP_CONNECTION_STATUS_FINWAIT2:
    case TCP_CONNECTION_STATUS_CLOSEWAIT:
    case TCP_CONNECTION_STATUS_LASTACK:
      return tcp_recv_close(pkt, con, tcphdr);

    case TCP_CONNECTION_STATUS_CLOSED:
    default:
      break;
  }

  return 0;
}

static int tcp_send(struct netbuf *pkt)
{
  struct connection *con;

  con = tcp_search_connection_from_number(pkt->option.tcp.send.number);
  tcp_send_enqueue(con, TCP_HEADER_FLAG_PSH|TCP_HEADER_FLAG_ACK, 1460, 0, 0, pkt->size, pkt->top);

  return 0;
}

static int tcp_proc(struct netbuf *buf)
{
  struct connection *con;
  int ret = 0;

  switch (buf->cmd) {
    case TCP_CMD_IPADDR:
      my_ipaddr = buf->option.common.ipaddr.addr;
      break;

    case TCP_CMD_ACCEPT:
      con = kz_kmalloc(sizeof(*con));
      memset(con, 0, sizeof(*con));

      con->number     = ++number;
      con->id         = buf->option.tcp.accept.id;
      con->src_ipaddr = my_ipaddr;
      con->src_port   = buf->option.tcp.accept.port;
      con->snd_number = 1;
      con->seq_number = 1;
      con->ack_number = 0;
      con->status     = TCP_CONNECTION_STATUS_LISTEN;

      con->send_queue = NULL;
      con->recv_queue = NULL;
      con->send_queue_end = &con->send_queue;
      con->recv_queue_end = &con->recv_queue;

      kz_send(MSGBOX_ID_TCPCONLIST, 0, (char *)con);
      tcp_search_connection_from_number(0); /* 頭出し */
      break;

    case TCP_CMD_CONNECT:
      con = kz_kmalloc(sizeof(*con));
      memset(con, 0, sizeof(*con));

      con->number     = ++number;
      con->id         = buf->option.tcp.connect.id;
      con->src_ipaddr = my_ipaddr;
      con->dst_ipaddr = buf->option.tcp.connect.ipaddr;
      con->src_port   = port++;
      con->dst_port   = buf->option.tcp.connect.port;
      con->snd_number = 1;
      con->seq_number = 1;
      con->ack_number = 0;
      con->status     = TCP_CONNECTION_STATUS_SYNSENT;

      con->send_queue = NULL;
      con->recv_queue = NULL;
      con->send_queue_end = &con->send_queue;
      con->recv_queue_end = &con->recv_queue;

      kz_send(MSGBOX_ID_TCPCONLIST, 0, (char *)con);
      tcp_search_connection_from_number(0); /* 頭出し */

      /* SYNを送信 */
      tcp_makesendpkt(con, TCP_HEADER_FLAG_SYN, 1460, 1460, 1, 0, NULL);
      break;

    case TCP_CMD_CLOSE:
      con = tcp_search_connection_from_number(buf->option.tcp.close.number);
      if (!con) break;

      /* FINを送信 */
      tcp_send_enqueue(con, TCP_HEADER_FLAG_FINACK, 1460, 0, 0, 0, NULL);
      con->status = TCP_CONNECTION_STATUS_FINWAIT1;
      break;

    case TCP_CMD_RECV:
      ret = tcp_recv(buf);
      break;

    case TCP_CMD_SEND:
      ret = tcp_send(buf);
      break;

    default:
      break;
  }

  return ret;
}

int tcp_main(int argc, char *argv[])
{
  struct netbuf *buf;
  int ret;

  kz_send(MSGBOX_ID_TCPCONLIST, 0, NULL);

  buf = kz_kmalloc(sizeof(*buf));
  buf->cmd = IP_CMD_REGPROTO;
  buf->option.ip.regproto.protocol = IP_PROTOCOL_TCP;
  buf->option.ip.regproto.cmd      = TCP_CMD_RECV;
  buf->option.ip.regproto.id       = MSGBOX_ID_TCPPROC;
  kz_send(MSGBOX_ID_IPPROC, 0, (char *)buf);

  while (1) {
    kz_recv(MSGBOX_ID_TCPPROC, NULL, (char **)&buf);
    ret = tcp_proc(buf);
    if (!ret)  kz_kmfree(buf);
  }

  return 0;
}
