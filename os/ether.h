#ifndef _ETHER_H_INCLUDED_
#define _ETHER_H_INCLUDED_

int ether_init(int index);
void ether_intr_enable(int index);
void ether_intr_disable(int index);
int ether_checkintr(int index);
int ether_recv(int index, char *buf);
int ether_send(int index, int size, char *buf);

#endif
