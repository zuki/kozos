#ifndef _KOZOS_H_INCLUDED_
#define _KOZOS_H_INCLUDED_

#include "configure.h"

struct _kz_thread;

typedef int (*kz_func)(int argc, char *argv[]);
typedef void (*kz_handler)(int signo);
typedef void (*kz_prefunc)(uint32 id);
typedef int (*kz_dbgfunc)(struct _kz_thread *thp, int signo);

/* syscall */
uint32 kz_run(kz_func func, char *name, int pri, int argc, char *argv[]);
void kz_exit();
int kz_wait();
int kz_sleep();
int kz_wakeup(uint32 id);
uint32 kz_getid();
int kz_chpri(int pri);
int kz_send(uint32 id, int size, char *p);
int kz_recv(uint32 *idp, char **pp);
int kz_pending();
int kz_setsig(int signo);
int kz_sethandler(int signo, kz_handler handler);
int kz_precall(uint32 id, kz_prefunc func);
int kz_debug(kz_dbgfunc func);
void *kz_kmalloc(int size);
int kz_kmfree(void *p);

/* srvcall */
int kx_wakeup(uint32 id);
int kx_send(uint32 id, int size, char *p);
void *kx_kmalloc(int size);
int kx_kmfree(void *p);

/* library */
void kz_start(kz_func func, char *name, int pri, int argc, char *argv[]);
void kz_sysdown();
void kz_trap();
void kz_break();

/* startup function and interrupt handler */
int kozos_start(int argc, char *argv[]);
void thread_intr(int vec);

/* general thread */
extern uint32 outlog_id;
extern uint32 timerd_id;
extern uint32 extintr_id;
extern uint32 idle_id;
extern uint32 stubd_id;
int extintr_main(int argc, char *argv[]);
int idle_main(int argc, char *argv[]);
int stubd_main(int argc, char *argv[]);

/* user thread */
extern uint32 command0_id;
extern uint32 command1_id;
int command_main(int argc, char *argv[]);

#endif
