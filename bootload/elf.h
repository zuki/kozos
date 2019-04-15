#ifndef _ELF_H_INCLUDED_
#define _ELF_H_INCLUDED_

char *elf_load(char *buf);
long load_program(unsigned char block_number, char *buf);
char *elf_startaddr(void);

#endif
