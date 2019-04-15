#ifndef _XMODEM_H_INCLUDED_
#define _XMODEM_H_INCLUDED_

#define XMODEM_BLOCK_SIZE 128

long xmodem_recv(void);
long load_from_xmodem(void);

#endif
