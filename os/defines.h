#ifndef _DEFINES_H_INCLUDED_
#define _DEFINES_H_INCLUDED_

#define NULL ((void *) 0)         /* NULLポインタ定義         */
#define SERIAL_DEFAULT_DEVICE 1   /* 標準シリアルデバイス番号 */

typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned long   uint32;

typedef uint32  kz_thread_id_t;                     /* スレッドID */
typedef int (*kz_func_t)(int argc, char *argv[]);   /* スレッドのメイン関数の型 */
typedef void (*kz_handler_t)(void);                 /* 割込みハンドラの型 */

typedef enum {
  MSGBOX_ID_MSGBOX1 = 0,
  MSGBOX_ID_MSGBOX2,
  MSGBOX_ID_NUM,
} kz_msgbox_id_t;

#endif
