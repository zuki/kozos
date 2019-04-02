#include "kozos.h"
#include "syscall.h"
#include "thread.h"

void kz_syscall(kz_syscall_type_t type, kz_syscall_param_t *param)
{
  current->syscall.type = type;
  current->syscall.param = param;
  asm volatile ("trapa #0");
  return;
}

/* System Call */

uint32 kz_run(kz_func func, char *name, int pri, int argc, char *argv[])
{
  kz_syscall_param_t param;
  param.un.run.func = func;
  param.un.run.name = name;
  param.un.run.pri = pri;
  param.un.run.argc = argc;
  param.un.run.argv = argv;
  kz_syscall(KZ_SYSCALL_TYPE_RUN, &param);
  return param.un.run.ret;
}

void kz_exit()
{
  kz_syscall(KZ_SYSCALL_TYPE_EXIT, NULL);
  return;
}

int kz_wait()
{
  kz_syscall_param_t param;
  kz_syscall(KZ_SYSCALL_TYPE_WAIT, &param);
  return param.un.wait.ret;
}

int kz_sleep()
{
  kz_syscall_param_t param;
  kz_syscall(KZ_SYSCALL_TYPE_SLEEP, &param);
  return param.un.sleep.ret;
}

int kz_wakeup(uint32 id)
{
  kz_syscall_param_t param;
  param.un.wakeup.id = id;
  kz_syscall(KZ_SYSCALL_TYPE_WAKEUP, &param);
  return param.un.wakeup.ret;
}

uint32 kz_getid()
{
  kz_syscall_param_t param;
  kz_syscall(KZ_SYSCALL_TYPE_GETID, &param);
  return param.un.getid.ret;
}

int kz_chpri(int pri)
{
  kz_syscall_param_t param;
  param.un.chpri.pri = pri;
  kz_syscall(KZ_SYSCALL_TYPE_CHPRI, &param);
  return param.un.chpri.ret;
}

int kz_send(uint32 id, int size, char *p)
{
  kz_syscall_param_t param;
  param.un.send.id = id;
  param.un.send.size = size;
  param.un.send.p = p;
  kz_syscall(KZ_SYSCALL_TYPE_SEND, &param);
  return param.un.send.ret;
}

int kz_recv(uint32 *idp, char **pp)
{
  kz_syscall_param_t param;
  param.un.recv.idp = idp;
  param.un.recv.pp = pp;
  kz_syscall(KZ_SYSCALL_TYPE_RECV, &param);
  return param.un.recv.ret;
}

int kz_pending()
{
  kz_syscall_param_t param;
  kz_syscall(KZ_SYSCALL_TYPE_PENDING, &param);
  return param.un.pending.ret;
}

int kz_setsig(int signo)
{
  kz_syscall_param_t param;
  param.un.setsig.signo = signo;
  kz_syscall(KZ_SYSCALL_TYPE_SETSIG, &param);
  return param.un.setsig.ret;
}

int kz_sethandler(int signo, kz_handler handler)
{
  kz_syscall_param_t param;
  param.un.sethandler.signo = signo;
  param.un.sethandler.handler = handler;
  kz_syscall(KZ_SYSCALL_TYPE_SETHANDLER, &param);
  return param.un.sethandler.ret;
}

int kz_precall(uint32 id, kz_prefunc func)
{
  kz_syscall_param_t param;
  param.un.precall.id = id;
  param.un.precall.func = func;
  kz_syscall(KZ_SYSCALL_TYPE_PRECALL, &param);
  return param.un.precall.ret;
}

int kz_debug(kz_dbgfunc func)
{
  kz_syscall_param_t param;
  param.un.debug.func = func;
  kz_syscall(KZ_SYSCALL_TYPE_DEBUG, &param);
  return param.un.debug.ret;
}

void *kz_kmalloc(int size)
{
  kz_syscall_param_t param;
  param.un.kmalloc.size = size;
  kz_syscall(KZ_SYSCALL_TYPE_KMALLOC, &param);
  return param.un.kmalloc.ret;
}

int kz_kmfree(void *p)
{
  kz_syscall_param_t param;
  param.un.kmfree.p = p;
  kz_syscall(KZ_SYSCALL_TYPE_KMFREE, &param);
  return param.un.kmfree.ret;
}

/* Service Call */

int kx_wakeup(uint32 id)
{
  kz_syscall_param_t param;
  param.un.wakeup.id = id;
  kz_srvcall(KZ_SYSCALL_TYPE_WAKEUP, &param);
  return param.un.wakeup.ret;
}

int kx_send(uint32 id, int size, char *p)
{
  kz_syscall_param_t param;
  param.un.send.id = id;
  param.un.send.size = size;
  param.un.send.p = p;
  kz_srvcall(KZ_SYSCALL_TYPE_SEND, &param);
  return param.un.send.ret;
}

void *kx_kmalloc(int size)
{
  kz_syscall_param_t param;
  param.un.kmalloc.size = size;
  kz_srvcall(KZ_SYSCALL_TYPE_KMALLOC, &param);
  return param.un.kmalloc.ret;
}

int kx_kmfree(void *p)
{
  kz_syscall_param_t param;
  param.un.kmfree.p = p;
  kz_srvcall(KZ_SYSCALL_TYPE_KMFREE, &param);
  return param.un.kmfree.ret;
}
