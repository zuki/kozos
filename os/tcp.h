#ifndef _TCP_H_INCLUDED_
#define _TCP_H_INCLUDED_

#define TCP_CMD_IPADDR    'i'    /* IPアドレスの通知       */
#define TCP_CMD_ACCEPT    'a'    /* コネクションの受け付け */
#define TCP_CMD_CONNECT   'c'    /* コネクションの作成     */
#define TCP_CMD_ESTAB     'e'    /* コネクションの確立     */
#define TCP_CMD_CLOSE     'l'    /* コネクションの終了     */
#define TCP_CMD_IPRECV    'R'    /* IPパケットの受信       */
#define TCP_CMD_RECV      'r'    /* IPストリームの受信     */
#define TCP_CMD_SEND      's'    /* IPストリームの送信     */

#endif
