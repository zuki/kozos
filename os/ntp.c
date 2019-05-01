#include "defines.h"
#include "kozos.h"
#include "consdrv.h"
#include "netdrv.h"
#include "udp.h"
#include "ntp.h"
#include "lib.h"

const unsigned char days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static void divide(uint32 epoch, uint32 divider, uint32 *qr)
{
  uint32 m = 1, q = 0;

  while (divider <= epoch) {
    divider <<= 1;
    m <<= 1;
  }
  while (1 < m) {
    divider >>= 1;
    m >>= 1;
    if (epoch >= divider) {
      epoch -= divider;
      q |= m;
    }
  }
  qr[0] = q;
  qr[1] = epoch;
}

static void localtime(uint32 epoch, struct tm *time)
{
  unsigned char i;
  uint32 qr[2];

  divide(epoch, 60, qr);
  time->sec = (unsigned char)(qr[1]&0xff);
  epoch = qr[0];
  divide(epoch, 60, qr);
  time->min = (unsigned char)(qr[1]&0xff);
  epoch = qr[0];
  epoch += TIMEZONE_JST;      /* タイムゾーン分加える */
  divide(epoch, 24, qr);
  time->hour = (unsigned char)(qr[1]&0xff);
  epoch = qr[0];
  epoch  -= (365UL * 100) + 23;   /* 1900から2000年までの日数を引く */
                                  /* 1900年代にうるう年は24回あるが */
                                  /* １日ずれるのでここで調整した   */
  time->year = 0;
  while(1) {
    if (epoch <= (365 * 4) + 1) { /* 残り4年以内 */
      if (epoch <= 365 + 1) break;
      epoch -= 365 + 1;
      time->year++;                  /* 残り3年以内 */
      if (epoch <= 365) break;
      epoch -= 365;
      time->year++;                  /* 残り2年以内 */
      if (epoch <= 365) break;
      epoch -= 365;
      time->year++;                  /* 残り1年以内 */
      break;
    }
    epoch -= (365 * 4) + 1;       /* 4年分処理 */
    time->year += 4;
  }
  for (i = 0; i < 12; i++) {
    if (epoch <= days[i]) break;
    if ((i == 1) && (epoch == 29) && ((time->year % 4) == 0)) break;
    epoch -= days[i];
  }
  time->month = i + 1;
  time->day   = epoch;
}

static void setvalue(unsigned char elem, int idx1, int idx2, unsigned char *str)
{
  unsigned char q = elem / 10, r = elem % 10;
  str[idx1] = '0' + q;
  str[idx2] = '0' + r;
}

static unsigned char *asctime(struct tm *time)
{
  unsigned char *str = "20YY/MM/DD HH:mm:SS JST\n";
  setvalue(time->year,  2,  3, str);
  setvalue(time->month, 5,  6, str);
  setvalue(time->day,   8,  9, str);
  setvalue(time->hour, 11, 12, str);
  setvalue(time->min,  14, 15, str);
  setvalue(time->sec,  17, 18, str);
  return str;
}

static void send_regport(int port)
{
  struct netbuf *buf;

  buf = kz_kmalloc(sizeof(*buf));
  buf->cmd = UDP_CMD_REGPORT;
  buf->option.udp.regport.src_port = port;
  buf->option.udp.regport.dst_port = port;
  buf->option.udp.regport.ipaddr   = 0;
  buf->option.udp.regport.id = MSGBOX_ID_NTP;
  kz_send(MSGBOX_ID_UDPPROC, 0, (char *)buf);
}

static void ntp_recv(struct netbuf *pkt)
{
  char *p;
  int len;
  struct tm time;
  unsigned char *daytime;
  struct ntp *ntphdr = (struct ntp *)pkt->top;
  localtime(ntphdr->transmit_timestamp.seconds, &time);
  daytime = asctime(&time);
  len = strlen(daytime);
  p = kz_kmalloc(len + 2);
  p[0] = '0';
  p[1] = CONSDRV_CMD_WRITE;
  memcpy(&p[2], daytime, len);
  kz_send(MSGBOX_ID_CONSOUTPUT, len + 2, p);
}

static void ntp_send(struct netbuf *pkt)
{
  struct netbuf *buf;
  struct ntp *ntphdr;

  buf = kz_kmalloc(DEFAULT_NETBUF_SIZE);
  memset(buf, 0, DEFAULT_NETBUF_SIZE);

  buf->cmd = UDP_CMD_SEND;
  buf->top = buf->data + 64;
  buf->size = sizeof(struct ntp);
  buf->option.udp.send.src_port = NTP_PORT;
  buf->option.udp.send.dst_port = NTP_PORT;
  buf->option.udp.send.ipaddr   = pkt->option.ntp.send.ipaddr;

  ntphdr = (struct ntp *)buf->top;
  ntphdr->flags = DEFAULT_NTP_FLAGS;

  kz_send(MSGBOX_ID_UDPPROC, 0, (char *)buf);
}

int ntp_main(int argc, char *argv[])
{
  struct netbuf *buf;

  send_regport(NTP_PORT);

  while (1) {
    kz_recv(MSGBOX_ID_NTP, NULL, (char **)&buf);
    switch (buf->cmd) {
    case NTP_CMD_SEND:
      ntp_send(buf);
      break;
    case UDP_CMD_RECV:
      ntp_recv(buf);
      break;
    default:
      break;
    }
    kz_kmfree(buf);
  }

  return 0;
}
