#ifndef _UDP_H_INCLUDED_
#define _UDP_H_INCLUDED_

#define UDP_CMD_IPADDR    'i' /* IPアドレスの通知 */
#define UDP_CMD_REGPORT   'a' /* ポート番号の登録 */
#define UDP_CMD_RELPORT   'l' /* ポート番号の解放 */
#define UDP_CMD_IPRECV    'R' /* IPパケットの受信 */
#define UDP_CMD_RECV      'r' /* UDPデータグラムの受信 */
#define UDP_CMD_SEND      's' /* UDPデータグラムの送信 */

struct udp_header {
  uint16  src_port;
  uint16  dst_port;
  uint16  length;
  uint16  checksum;
};

#endif
