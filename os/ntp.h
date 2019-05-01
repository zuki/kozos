#ifndef _NTP_H_INCLUDED_
#define _NTP_H_INCLUDED_

/* #define NTP_SERVER  {133,243,238,243} ntp.nict.jp*/
#define NTP_SERVER 0xdb5efc9c;  /* 219.94.252.156 */
#define NTP_PORT   123

#define TIMEZONE_JST   9     /* JSTタイムゾーン数値 */
#define NTP_CMD_SEND  'D'    /* NTPコマンドの送信   */
#define NTP_CMD_RECV  'R'    /* NTPコマンドの受信   */
#define NTP_CMD_DSPLY 'D'    /* 日付の表示          */

struct timestamp {
  uint32  seconds;
  uint32  fraction;
};

struct ntp {
  unsigned char     flags;
  unsigned char     strautum;
  unsigned char     pol_interval;
  unsigned char     precision;
  uint32            root_delay;
  uint32            root_dispersion;
  unsigned char     reference_identifier[4];
  struct timestamp  reference_timestamp;
  struct timestamp  originate_timestamp;
  struct timestamp  receive_timestamp;
  struct timestamp  transmit_timestamp;
};

#define NTP_FLAGS_LI   (0 << 6)
#define NTP_FLAGS_VN   (3 << 3)
#define NTP_FLAGS_MODE (3 << 0)

#define DEFAULT_NTP_FLAGS   (NTP_FLAGS_LI | NTP_FLAGS_VN | NTP_FLAGS_MODE)

struct tm {
  unsigned char year;   /* 下二桁  */
  unsigned char month;  /* 1-12    */
  unsigned char day;    /* 1-31    */
  unsigned char hour;   /* 0-23    */
  unsigned char min;    /* 0-59    */
  unsigned char sec;    /* 0-59    */
};

#endif
