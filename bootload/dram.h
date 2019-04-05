#ifndef _DRAM_H_INCLUDED_
#define _DRAM_H_INCLUDED_

#define DRAM_START  0x400000
#define DRAM_END    0x600000

int dram_init(void);
int dram_check(void);
int dram_check2(void);
int dram_clear(void);

#endif
