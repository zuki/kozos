#include "kozos.h"
#include "syscall.h"
#include "memory.h"
#include "thread.h"
#if 0
#include "stublib.h"
#endif

#include "interrupt.h"
#include "lib.h"

#define SIG_NUM 32

kz_thread threads[THREAD_NUM];
static struct {
  kz_thread *head;
  kz_thread *tail;
} readyque[PRI_NUM];
static uint32 readyque_bitmap;
static kz_thread *sigcalls[SIG_NUM];
static kz_handler handlers[SIG_NUM];
static kz_dbgfunc debug_handler = NULL;
static kz_prefunc default_precall = NULL;

kz_thread *current;

static char *thread_stack = (char *)THREAD_STACK_START;

static int getcurrent()
{
  if (current == NULL) {
    return -1;
  }
  if (!(current->flags & KZ_THREAD_FLAG_RUNNING)) {
    /* すでに無い場合は無視 */
    return 1;
  }

  readyque[current->pri].head = current->next;
  if (readyque[current->pri].head == NULL) {
    readyque[current->pri].tail = NULL;
    readyque_bitmap &= ~((uint32)1 << current->pri);
  }
  current->flags &= ~KZ_THREAD_FLAG_RUNNING;
  current->next = NULL;

  return 0;
}

static int putcurrent()
{
  if (current == NULL) {
    return -1;
  }
  if (current->flags & KZ_THREAD_FLAG_RUNNING) {
    /* すでに有る場合は無視 */
    return 1;
  }

  if (readyque[current->pri].tail) {
    readyque[current->pri].tail->next = current;
  } else {
    readyque[current->pri].head = current;
  }
  readyque[current->pri].tail = current;
  readyque_bitmap |= ((uint32)1 << current->pri);
  current->flags |= KZ_THREAD_FLAG_RUNNING;

  return 0;
}

static void thread_end()
{
  kz_exit();
}

static void thread_init(kz_thread *thp, int argc, char *argv[])
{
  thp->func(argc, argv);
  thread_end();
}

static uint32 thread_run(kz_func func, char *name, int pri,
			 int argc, char *argv[])
{
  int i;
  kz_thread *thp;

  for (i = 0; i < THREAD_NUM; i++) {
    thp = &threads[i];
    if (!thp->id) break;
  }
  if (i == THREAD_NUM) return -1;

  memset(thp, 0, sizeof(*thp));

  thp->next = NULL;
  strcpy(thp->name, name);
  thp->id = thp;
  thp->func = func;
  thp->pri = pri;

  memset(thread_stack, 0, SIGSTKSZ);

  thread_stack += THREAD_STACK_SIZE;
  thp->stack = thread_stack;

  thp->context.pc = (uint32)thread_init | ((uint32)(pri ? 0 : 0xc0) << 24);

  thp->context.er[0] = (uint32)thp;
  thp->context.er[1] = (uint32)argc;
  thp->context.er[2] = (uint32)argv;

  thp->context.er[7] = (uint32)thp->stack;

  thp->context.er[7] -= 4;
  *(uint32 *)thp->context.er[7] = (uint32)thread_end;
  thp->context.er[7] -= 4;
  *(uint32 *)thp->context.er[7] = thp->context.pc;
  thp->context.er[7] -= 4;
  *(uint32 *)thp->context.er[7] = thp->context.er[0];

  /* 起動時の初回のスレッド作成では current 未定なのでNULLチェックする */
  if (current) {
    putcurrent();
  }

  current = thp;
  putcurrent();

  return (uint32)current;
}

/* スレッドの終了 */
static int thread_exit()
{
  memset(current, 0, sizeof(*current));
  return 0;
}

/* スレッドの実行権放棄(同一priの別スレッドに実行権を渡す) */
static int thread_wait()
{
  putcurrent();
  return 0;
}

static int thread_sleep()
{
  return 0;
}

static int thread_wakeup(uint32 id)
{
  putcurrent();
  current = (kz_thread *)id;
  putcurrent();
  return 0;
}

static uint32 thread_getid()
{
  putcurrent();
  return (uint32)current->id;
}

static int thread_chpri(int pri)
{
  int old = current->pri;
  if (pri >= 0)
    current->pri = pri;
  putcurrent();
  return old;
}

static void recvmsg()
{
  kz_msgbuf *mp;

  mp = current->messages.head;
  current->messages.head = mp->next;
  if (current->messages.head == NULL)
    current->messages.tail = NULL;
  mp->next = NULL;

  current->syscall.param->un.recv.ret = mp->size;
  if (current->syscall.param->un.recv.idp)
    *(current->syscall.param->un.recv.idp) = mp->id;
  if (current->syscall.param->un.recv.pp)
    *(current->syscall.param->un.recv.pp)  = mp->p;
  kzmem_free(mp);
}

static void sendmsg(kz_thread *thp, uint32 id, int size, char *p)
{
  kz_msgbuf *mp;

  current = thp;

  mp = (kz_msgbuf *)kzmem_alloc(sizeof(*mp));
  if (mp == NULL) {
    while (1)
      ;
  }
  mp->next = NULL;
  mp->size = size;
  mp->id = id;
  mp->p = p;

  if (current->messages.tail) {
    current->messages.tail->next = mp;
  } else {
    current->messages.head = mp;
  }
  current->messages.tail = mp;

  if (putcurrent() == 0) {
    /* 受信する側がブロック中の場合には受信処理を行う */
    recvmsg();
  }
}

static int thread_send(uint32 id, int size, char *p)
{
  putcurrent();
  sendmsg((kz_thread *)id, (uint32)current, size, p);
  return size;
}

static int thread_recv(uint32 *idp, char **pp)
{
  if (current->messages.head == NULL) {
    /* メッセージが無いのでブロックする */
    return -1;
  }

  recvmsg();
  putcurrent();
  return current->syscall.param->un.recv.ret;
}

static int thread_pending()
{
  putcurrent();
  return current->messages.head ? 1 : 0;
}

static int thread_setsig(int signo)
{
  sigcalls[signo] = current;
  putcurrent();
  return 0;
}

static int thread_sethandler(int signo, kz_handler handler)
{
  handlers[signo] = handler;
  putcurrent();
  return 0;
}

static void extintr_proc(int signo)
{
  if (handlers[signo])
    handlers[signo](signo);
  if (sigcalls[signo])
    sendmsg(sigcalls[signo], 0, 0, NULL);
}

static int thread_precall(uint32 id, kz_prefunc func)
{
  kz_thread *thp = (kz_thread *)id;
  if (thp)
    thp->precall = func;
  else
    default_precall = func;
  putcurrent();
  return 0;
}

static int thread_debug(kz_dbgfunc func)
{
  debug_handler = func;
#if 0
  stub_init();
#endif
  putcurrent();
  return 0;
}

static void *thread_kmalloc(int size)
{
  putcurrent();
  return kzmem_alloc(size);
}

static int thread_kmfree(char *p)
{
  kzmem_free(p);
  putcurrent();
  return 0;
}

static void syscall_procedure(kz_syscall_type_t type, kz_syscall_param_t *p)
{
  /* システムコールの実行中にcurrentが書き換わるので注意 */
  switch (type) {
  case KZ_SYSCALL_TYPE_RUN:
    p->un.run.ret = thread_run(p->un.run.func, p->un.run.name, p->un.run.pri,
			       p->un.run.argc, p->un.run.argv);
    break;
  case KZ_SYSCALL_TYPE_EXIT:
    /* スレッドが解放されるので戻り値などを書き込んではいけない */
    thread_exit();
    break;
  case KZ_SYSCALL_TYPE_WAIT:
    p->un.wait.ret = thread_wait();
    break;
  case KZ_SYSCALL_TYPE_SLEEP:
    p->un.sleep.ret = thread_sleep();
    break;
  case KZ_SYSCALL_TYPE_WAKEUP:
    p->un.wakeup.ret = thread_wakeup(p->un.wakeup.id);
    break;
  case KZ_SYSCALL_TYPE_GETID:
    p->un.getid.ret = thread_getid();
    break;
  case KZ_SYSCALL_TYPE_CHPRI:
    p->un.chpri.ret = thread_chpri(p->un.chpri.pri);
    break;
  case KZ_SYSCALL_TYPE_SEND:
    p->un.send.ret = thread_send(p->un.send.id, p->un.send.size, p->un.send.p);
    break;
  case KZ_SYSCALL_TYPE_RECV:
    p->un.recv.ret = thread_recv(p->un.recv.idp, p->un.recv.pp);
    break;
  case KZ_SYSCALL_TYPE_PENDING:
    p->un.pending.ret = thread_pending();
    break;
  case KZ_SYSCALL_TYPE_SETSIG:
    p->un.setsig.ret = thread_setsig(p->un.setsig.signo);
    break;
  case KZ_SYSCALL_TYPE_SETHANDLER:
    p->un.sethandler.ret = thread_sethandler(p->un.sethandler.signo,
					     p->un.sethandler.handler);
    break;
  case KZ_SYSCALL_TYPE_PRECALL:
    p->un.precall.ret = thread_precall(p->un.precall.id, p->un.precall.func);
    break;
  case KZ_SYSCALL_TYPE_DEBUG:
    p->un.debug.ret = thread_debug(p->un.debug.func);
    break;
  case KZ_SYSCALL_TYPE_KMALLOC:
    p->un.kmalloc.ret = thread_kmalloc(p->un.kmalloc.size);
    break;
  case KZ_SYSCALL_TYPE_KMFREE:
    p->un.kmfree.ret = thread_kmfree(p->un.kmfree.p);
    break;
  default:
    break;
  }
  return;
}

static void syscall_proc(kz_syscall_type_t type, kz_syscall_param_t *p)
{
  getcurrent();
  syscall_procedure(type, p);
}

static void srvcall_proc(kz_syscall_type_t type, kz_syscall_param_t *p)
{
  /*
   * サービスコールなので current をNULLに設定する．(システムコールとサービス
   * コールの内部で，システムコールの実行したスレッドIDを得るために current を
   * 参照している部分があり(たとえば thread_send() など)，current が残って
   * いると誤動作する)
   * サービスコール呼び出し後にスケジューリング処理が行われ，
   * current は再設定される．
   */
  current = NULL;
  syscall_procedure(type, p);
}

static void schedule()
{
#if 0
#if PRI_NUM > 32
#error ビットマップを配列化する必要あり
#endif
  uint32 bitmap = readyque_bitmap;
  int n = 0;
  static int bitmap2num[16] = {
    -32, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
  };

  if (!(bitmap & 0xffff)) {
    bitmap >>= 16;
    n += 16;
  }
  if (!(bitmap & 0xff)) {
    bitmap >>= 8;
    n += 8;
  }
  if (!(bitmap & 0xf)) {
    bitmap >>= 4;
    n += 4;
  }
  n += bitmap2num[bitmap & 0xf];
  if (n < 0) {
    /* 実行可能なスレッドが存在しないので，終了する */
    while (1)
      ;
  }
#else
  int n;
  for (n = 0; n < PRI_NUM; n++)
    if (readyque[n].head) break;
  if (n == PRI_NUM)
    while (1)
      ;
#endif

  current = readyque[n].head;
}

static void thread_intrvec(int signo, kz_thread *thp)
{
  kz_thread *current_old;

  switch (signo) {
  case SIGSYS: /* システムコール */
    syscall_proc(thp->syscall.type, thp->syscall.param);
    break;
  case SIGHUP: /* 外部割込み */
    break;
  case SIGALRM: /* タイマ割込み発生 */
    /* none */
    break;
  case SIGBUS: /* ダウン要因発生 */
  case SIGSEGV:
  case SIGTRAP:
  case SIGILL:
    if (debug_handler) {
      debug_handler(thp, signo);
    } else {
      /* ダウン要因発生により継続不可能なので，スリープ状態にする*/
      getcurrent();
#if 1 /* スレッド終了する */
      thread_exit();
#endif
    }
    break;
  default:
    break;
  }
  extintr_proc(signo);

  current_old = NULL;
  while (1) {
    schedule();

    if (current_old && (current == current_old))
      break;

    current_old = current;

    if (current->precall) {
      current->precall((uint32)current);
    } else if (default_precall) {
      default_precall((uint32)current);
    } else {
      break;
    }

    /*
     * 上記 precall の呼び出し内部でサービスコールが呼ばれて current が
     * 書きかわっていたり，もっと優先度の高いスレッドがレディー状態に
     * なっている可能性があるので，スケジューリングしなおす．
     */
  }

  dispatch(&current->context);
}

void thread_intr(int vec)
{
  kz_thread *thp = current;
  int signo;
  uint32 *p;

  switch (vec) {
#if 0
  case 0x04: signo = SIGBUS;  break;
  case 0x05: signo = SIGHUP;  break;
  case 0x07: signo = SIGTRAP; break;
  case 0x09: signo = SIGALRM; break;
#endif
  case VECTYPE_RXI0:
  case VECTYPE_RXI1:
  case VECTYPE_RXI2: signo = SIGHUP;  break;
  case VECTYPE_TRAPA0: signo = SIGSYS;  break;
  default:
    signo = SIGBUS;
    break;
  }

  p = (uint32 *)INTR_STACK_START;
  current->context.er[7] = *(--p);
  current->context.er[1] = *(--p);
  current->context.er[2] = *(--p);
  current->context.er[3] = *(--p);
  current->context.er[4] = *(--p);
  current->context.er[5] = *(--p);
  current->context.er[6] = *(--p);
  current->context.er[0] = *(uint32 *)(current->context.er[7]);

  current->context.pc = *(uint32 *)(current->context.er[7] + 4);

  thread_intrvec(signo, thp);
  current = thp;
}

static void thread_start(kz_func func, char *name, int pri, int argc, char *argv[])
{
  memset(threads, 0, sizeof(threads));
  memset(readyque, 0, sizeof(readyque));
  memset(sigcalls, 0, sizeof(sigcalls));
  memset(handlers, 0, sizeof(handlers));

  readyque_bitmap = 0;

  /*
   * current 未定のためにシステムコール発行はできないので，
   * 直接関数を呼び出してスレッド作成する．
   */
  current = NULL;
  current = (kz_thread *)thread_run(func, name, pri, argc, argv);

  dispatch(&current->context);
}

void kz_start(kz_func func, char *name, int pri, int argc, char *argv[])
{
  kzmem_init();

  thread_start(func, name, pri, argc, argv);

  /* ここには返ってこない */
  while (1)
    ;
}

void kz_sysdown()
{
  while (1)
    ;
}

void breakpoint();

void kz_trap()
{
  asm volatile ("trapa #0");
}

#if 0
void kz_break()
{
  breakpoint();
}
#endif

void kz_srvcall(kz_syscall_type_t type, kz_syscall_param_t *param)
{
  srvcall_proc(type, param);
  return;
}
