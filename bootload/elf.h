#ifndef _ELF_H_INCLUDED_
#define _ELF_H_INCLUDED_

char *elf_load(char *buf);
int load_program(unsigned char block_number, char *buf);
char *elf_startaddr(void);

#endif
