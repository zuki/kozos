#ifndef _KOZOS_CONFIGURE_H_INCLUDED_
#define _KOZOS_CONFIGURE_H_INCLUDED_

#include "types.h"
#include "lib.h"

extern uint32 intrstack;
#define INTR_STACK_START   (&intrstack)
#define THREAD_STACK_START 0xffe820
#define THREAD_STACK_SIZE  0x300
#define SIGSTKSZ THREAD_STACK_SIZE

#define SIGHUP   1
#define SIGILL   4
#define SIGTRAP  5
#define SIGBUS  10
#define SIGSEGV 11
#define SIGSYS  12
#define SIGALRM 14

typedef int time_t;

struct timeval {
  time_t tv_sec;
  time_t tv_usec;
};

#endif
