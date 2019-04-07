#ifndef _IP_H_INCLUDED_
#define _IP_H_INCLUDED_

#define IP_CMD_REGPROTO   'p'   /* プロトコル番号の登録 */
#define IP_CMD_RECV       'r'   /* IPパケットの受信     */
#define IP_CMD_SEND       's'   /* IPパケットの送信     */

uint16 ip_calc_checksum(int size, void *buf);

#endif
