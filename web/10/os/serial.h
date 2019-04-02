#ifndef _SERIAL_H_INCLUDED_
#define _SERIAL_H_INCLUDED_

int serial_initialize(int baudrate, int clk);
int serial_init(int index);
int serial_putb(int index, int b);
int serial_putc(int index, int c);
int serial_tstc(int index);
int serial_getb(int index);
int serial_getc(int index);
void serial_intr_enable(int index);
void serial_intr_disable(int index);

#endif
