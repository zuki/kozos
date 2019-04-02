#ifndef _KOZOS_THREAD_H_INCLUDED_
#define _KOZOS_THREAD_H_INCLUDED_

#include "kozos.h"
#include "syscall.h"

#define THREAD_NUM 6
#define PRI_NUM 32
#define THREAD_NAME_SIZE 15

typedef struct _kz_msgbuf {
  struct _kz_msgbuf *next;
  uint32 id;
  int size;
  char *p;
} kz_msgbuf;

typedef struct _kz_context {
  uint32 er[8];
  uint32 pc;
  char dummy[28];
} kz_context;

typedef struct _kz_thread {
  struct _kz_thread *next;
  char name[THREAD_NAME_SIZE + 1];
  struct _kz_thread *id;
  kz_func func;
  int pri;
  char *stack;
  uint32 flags;
#define KZ_THREAD_FLAG_RUNNING (1 << 0)

  kz_prefunc precall;

  struct {
    kz_syscall_type_t type;
    kz_syscall_param_t *param;
  } syscall;

  struct {
    kz_msgbuf *head;
    kz_msgbuf *tail;
  } messages;

  kz_context context;
  char dummy[4];
} kz_thread;

extern kz_thread threads[THREAD_NUM];
extern kz_thread *current;

void dispatch(kz_context *context);

#endif
