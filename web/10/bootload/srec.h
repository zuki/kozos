#ifndef _SREC_H_INCLUDED_
#define _SREC_H_INCLUDED_

int srec_init();
int srec_decode(unsigned char c);
char *srec_startaddr();

#endif
