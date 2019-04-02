#ifndef _KOZOS_SYSCALL_H_INCLUDED_
#define _KOZOS_SYSCALL_H_INCLUDED_

#include "kozos.h"

typedef enum {
  KZ_SYSCALL_TYPE_RUN,
  KZ_SYSCALL_TYPE_EXIT,
  KZ_SYSCALL_TYPE_WAIT,
  KZ_SYSCALL_TYPE_SLEEP,
  KZ_SYSCALL_TYPE_WAKEUP,
  KZ_SYSCALL_TYPE_GETID,
  KZ_SYSCALL_TYPE_CHPRI,
  KZ_SYSCALL_TYPE_SEND,
  KZ_SYSCALL_TYPE_RECV,
  KZ_SYSCALL_TYPE_PENDING,
  KZ_SYSCALL_TYPE_SETSIG,
  KZ_SYSCALL_TYPE_SETHANDLER,
  KZ_SYSCALL_TYPE_PRECALL,
  KZ_SYSCALL_TYPE_DEBUG,
  KZ_SYSCALL_TYPE_KMALLOC,
  KZ_SYSCALL_TYPE_KMFREE,
} kz_syscall_type_t;

typedef struct {
  union {
    struct {
      kz_func func;
      char *name;
      int pri;
      int argc;
      char **argv;
      int ret;
    } run;
    struct {
      int dummy;
    } exit;
    struct {
      int ret;
    } wait;
    struct {
      int ret;
    } sleep;
    struct {
      uint32 id;
      int ret;
    } wakeup;
    struct {
      uint32 ret;
    } getid;
    struct {
      int pri;
      int ret;
    } chpri;
    struct {
      uint32 id;
      int size;
      char *p;
      int ret;
    } send;
    struct {
      uint32 *idp;
      char **pp;
      int ret;
    } recv;
    struct {
      int ret;
    } pending;
    struct {
      int signo;
      int ret;
    } setsig;
    struct {
      int signo;
      kz_handler handler;
      int ret;
    } sethandler;
    struct {
      uint32 id;
      kz_prefunc func;
      int ret;
    } precall;
    struct {
      kz_dbgfunc func;
      int ret;
    } debug;
    struct {
      int size;
      void *ret;
    } kmalloc;
    struct {
      char *p;
      int ret;
    } kmfree;
  } un;
} kz_syscall_param_t;

void kz_syscall(kz_syscall_type_t type, kz_syscall_param_t *param);
void kz_srvcall(kz_syscall_type_t type, kz_syscall_param_t *param);

#endif
