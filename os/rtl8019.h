#ifndef _RTL8019_H_INCLUDED_
#define _RTL8019_H_INCLUDED_

int rtl8019_init(int index, unsigned char macaddr[]);
char *rtl8019_macaddr(int index);
void rtl8019_intr_enable(int index);
void rtl8019_intr_disable(int index);
int rtl8019_checkintr(int index);
int rtl8019_recv(int index, char *buf);
int rtl8019_send(int index, int size, char *buf);
int rtl8019_clearintr(int index);

#endif
