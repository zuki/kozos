#ifndef _SERIAL_H_INCLUDED_
#define _SERIAL_H_INCLUDED_

int serial_init();
int serial_putb(int b);
int serial_putc(int c);
int serial_tstc();
int serial_getb();
int serial_getc();

#endif
