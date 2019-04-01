#include "defines.h"
#include "kozos.h"
#include "intr.h"
#include "interrupt.h"
#include "syscall.h"
#include "memory.h"
#include "lib.h"

#define THREAD_NUM         6
#define PRIORITY_NUM      16
#define THREAD_NAME_SIZE  15

/* スレッドコンテキスト */
typedef struct _kz_context {
  uint32 sp;
} kz_context;

/* タスク・コントロール・ブロック (TCB) */
typedef struct _kz_thread {
  struct _kz_thread *next;
  char name[THREAD_NAME_SIZE + 1];
  int priority;   /* 優先度 */
  char *stack;    /* スタック */
  uint32 flags;   /* 各種フラグ */
#define KZ_THREAD_FLAG_READY (1 << 0)

  struct {  /* スレッドのスタートアップ(thread_init())に渡すパラメタ */
    kz_func_t func;   /* スレッドのメイン関数 */
    int argc;         /* スレッドのメイン関数に渡す argc */
    char **argv;      /* スレッドのメイン関数に渡す argv */
  } init;     /* 12 = 4 + 2 + d2 + 4 */

  struct {  /* システムコール用バッファ */
    kz_syscall_type_t type;
    kz_syscall_param_t *param;
  } syscall;  /* 8 = 2 + d2 + 4 */

  kz_context context;   /* コンテキスト情報 */
  char dummy[8];        /* __mulsi3 対応 = 56 + 8 = 2 ^ 6 */
} kz_thread;

/* メッセージ・バッファ */
typedef struct _kz_msgbuf {
  struct _kz_msgbuf *next;
  kz_thread * sender;   /* メッセージを送信したスレッド */
  struct {              /* メッセージのパラメタ保存領域 */
    int size;
    char *p;
  } param;
} kz_msgbuf;

/* メッセージボックス */
typedef struct _kz_msgbox {
  kz_thread *receiver;  /* 受信待ち状態のスレッド */
  kz_msgbuf *head;
  kz_msgbuf *tail;
  char dummy[4];        /* __mulsi3 対応 */
} kz_msgbox;

/* スレッドのレディー・キュー */
static struct {
  kz_thread *head;
  kz_thread *tail;
} readyque[PRIORITY_NUM];

static kz_thread    *current;                   /* カレントスレッド */
static kz_thread    threads[THREAD_NUM];        /* タスクコントロールブロック (TCB) */
static kz_handler_t handlers[SOFTVEC_TYPE_NUM]; /* 割込みハンドラ */
static kz_msgbox    msgboxes[MSGBOX_ID_NUM];    /* メッセージボックス */

void dispatch(kz_context *context);             /* 実態は startup.s にアセンブラで記述 */

/* カレントスレッドをレディーキューから抜き出す */
static int getcurrent(void)
{
  if (current == NULL) {
    return -1;
  }

  if (!(current->flags & KZ_THREAD_FLAG_READY)) {
    /* すでにない場合は無視 */
    return 1;
  }

  /* カレントスレッドは必ず先頭にあるので、先頭から抜き出す */
  readyque[current->priority].head = current->next;
  if (readyque[current->priority].head == NULL) {
    readyque[current->priority].tail = NULL;
  }
  current->flags &= ~KZ_THREAD_FLAG_READY;
  current->next = NULL;

  return 0;
}

/* カレントスレッドをレディキューにつなげる */
static int putcurrent(void)
{
  if (current == NULL) {
    return -1;
  }

  if (current->flags & KZ_THREAD_FLAG_READY) {
    /* すでにある場合は無視 */
    return 1;
  }

  /* レディキューの末尾に接続する */
  if (readyque[current->priority].tail) {
    readyque[current->priority].tail->next = current;
  } else {
    readyque[current->priority].head = current;
  }
  readyque[current->priority].tail = current;
  current->flags |= KZ_THREAD_FLAG_READY;

  return 0;
}

static void thread_end(void)
{
  kz_exit();
}

/* スレッドのスタートアップ */
static void thread_init(kz_thread *thp)
{
  /* スレッドのメイン関数を呼び出す */
  thp->init.func(thp->init.argc, thp->init.argv);
  thread_end();
}

/* システムコールの処理 (kz_run(): スレッドの起動) */
static kz_thread_id_t thread_run(kz_func_t func, char *name, int priority,
                                int stacksize, int argc, char *argv[])
{
  int i;
  kz_thread *thp;
  uint32 *sp;
  extern char userstack;  /* リンカスクリプトで定義されているスタック領域 */
  static char *thread_stack = &userstack;

  /* 空いているTCBを検索 */
  for (i = 0; i < THREAD_NUM; i++) {
    thp = &threads[i];
    if (!thp->init.func) /* 見つかった */
      break;
  }
  if (i == THREAD_NUM) /* 見つからなかった */
    return -1;

  memset(thp, 0, sizeof(*thp));

  /* TCBの設定 */
  strcpy(thp->name, name);
  thp->next      = NULL;
  thp->priority  = priority;
  thp->flags     = 0;

  thp->init.func = func;
  thp->init.argc = argc;
  thp->init.argv = argv;

  /* スタック領域を獲得 */
  memset(thread_stack, 0, stacksize);
  thread_stack += stacksize;

  thp->stack = thread_stack;  /* スタックの設定 */

  /* スタックの初期化 */
  sp = (uint32 *)thp->stack;
  *(--sp) = (uint32)thread_end;

  /* プログラムカウンタを設定する
   * sレッドの優先度がゼロの場合には、割込み禁止スレッドとする
   */
  *(--sp) = (uint32)thread_init | ((uint32)(priority ? 0 : 0xc0) << 24);

  *(--sp) = 0; /* ER6 */
  *(--sp) = 0; /* ER5 */
  *(--sp) = 0; /* ER4 */
  *(--sp) = 0; /* ER3 */
  *(--sp) = 0; /* ER2 */
  *(--sp) = 0; /* ER1 */

  /* スレッドのスタートアップ (thread_init())に渡す引数 */
  *(--sp) = (uint32)thp;  /* ER0 */

  /* スレッドのコンテキストを設定 */
  thp->context.sp = (uint32)sp;

  /* システムコールを呼び出したスレッドをレディーキューに戻す */
  putcurrent();

  /* 新規作成したスレッドをレディキューに接続する */
  current = thp;
  putcurrent();

  return (kz_thread_id_t)current;
}

/* システムコールの処理 (kz_exit(): スレッドの終了) */
static int thread_exit(void)
{
  /*
   * 本来ならスタックも開放して再利用できるようにすべきだが省略。
   * このため、スレッドを頻繁に生成・消去するようなことは現状ではできない
   */
  puts(current->name);
  puts(" EXIT.\n");
  memset(current, 0, sizeof(*current));
  return 0;
}

/* システムコールの処理 (kz_wait(): スレッドの実行権放棄) */
static int thread_wait(void)
{
  putcurrent();
  return 0;
}

/* システムコールの処理 (kz_sleep(): スレッドのスリープ) */
static int thread_sleep(void)
{
  return 0;
}

/* システムコールの処理 (kz_wakup(): スレッドのウェイクアップ) */
static int thread_wakeup(kz_thread_id_t id)
{
  /* ウェイクアップを呼び出したスレッドをレディキューに戻す */
  putcurrent();

  /* 指定されたスレッドをレディキューに接続してウェイクアップする */
  current = (kz_thread *)id;
  putcurrent();

  return 0;
}

/* システムコールの処理 (kz_getid(): スレッドID取得) */
static kz_thread_id_t thread_getid(void)
{
  putcurrent();
  return (kz_thread_id_t)current;
}

/* システムコールの処理 (kz_chpri(): スレッドの優先度変更) */
static int thread_chpri(int priority)
{
  int old = current->priority;
  if (priority >= 0 && priority <= PRIORITY_NUM)
    current->priority = priority;   /* 優先度変更 */
  putcurrent(); /* 新しい優先度のレディキューににつなぎ直す */
  return old;
}

/* システムコールの処理 (kz_kmalloc(): 動的メモリ獲得) */
static void *thread_kmalloc(int size)
{
  putcurrent();
  return kzmem_alloc(size);
}

/* システムコールの処理 (kz_kmfree(): メモリ解放) */
static int thread_kmfree(char *p)
{
  kzmem_free(p);
  putcurrent();
  return 0;
}

/* メッセージの送信処理 */
static void sendmsg(kz_msgbox *mboxp, kz_thread *thp, int size, char *p)
{
  kz_msgbuf *mp;

  /* メッセージバッファの作成 */
  mp = (kz_msgbuf *)kzmem_alloc(sizeof(*mp));
  if (mp == NULL)
    kz_sysdown();
  mp->next        = NULL;
  mp->sender      = thp;
  mp->param.size  = size;
  mp->param.p     = p;

  /* メッセージボックスの末尾にメッセージを接続する */
  if (mboxp->tail) {
    mboxp->tail->next = mp;
  } else {
    mboxp->head = mp;
  }
  mboxp->tail = mp;
}

/* メッセージ受信処理 */
static void recvmsg(kz_msgbox *mboxp)
{
  kz_msgbuf *mp;
  kz_syscall_param_t *p;

  /* メッセージボックスの先頭にあるメッセージを抜き出す */
  mp = mboxp->head;
  mboxp->head = mp->next;
  if (mboxp->head == NULL)
    mboxp->tail = NULL;
  mp->next = NULL;

  /* メッセージを受信するスレッドに返す値を設定する */
  p = mboxp->receiver->syscall.param;
  p->un.recv.ret = (kz_thread_id_t)mp->sender;
  if (p->un.recv.sizep)
    *(p->un.recv.sizep) = mp->param.size;
  if (p->un.recv.pp)
    *(p->un.recv.pp) = mp->param.p;

  /* 受信待ちスレッドはいなくなっｔので、NULLに戻す */
  mboxp->receiver = NULL;

  /* メッセージバッファの開放 */
  kzmem_free(mp);
}

/* システムコールの処理 (kz_send(): メッセージ送信) */
static int thread_send(kz_msgbox_id_t id, int size, char *p)
{
  kz_msgbox *mboxp = &msgboxes[id];

  putcurrent();
  sendmsg(mboxp, current, size, p);   /* メッセージの送信処理 */

  /* 受信待ちスレッドが存在している場合には受信処理を行う */
  if (mboxp->receiver) {
    current = mboxp->receiver;    /* 受信待ちスレッド */
    recvmsg(mboxp);   /* メッセージの受信処理 */
    putcurrent();     /* 受信により動作可能になったので、プロック解除する */
  }

  return size;
}

/* システムコールの処理 (kz_recv(): メッセージ受信) */
static kz_thread_id_t thread_recv(kz_msgbox_id_t id, int *sizep, char **pp)
{
  kz_msgbox *mboxp = &msgboxes[id];

  if (mboxp->receiver)  /* 他のスレッドがすでに受信待ちしている */
    kz_sysdown();

  mboxp->receiver = current;  /* 受信待ちスレッドに設定 */

  if (mboxp->head == NULL) {
    /*
     * メッセージボックスにメッセージがないので、スレッドを
     * スリープさせる。（システムコールがブロックする）
     */
    return -1;
  }

  recvmsg(mboxp); /* メッセージの受信処理 */
  putcurrent();   /* メッセーー字を受信できたので、レディー状態にする */

  return current->syscall.param->un.recv.ret;

}

static void thread_intr(softvec_type_t type, unsigned long sp);

/* システムコールの処理 (kz_setintr(): 割込みハンドラ登録) */
static int thread_setintr(softvec_type_t type, kz_handler_t handler)
{
  /*
   * 割込みを受け付けるために、ソフトウェア割り込みベクタに
   * OSの割込み処理の入口となる関数を登録する
   */
  softvec_setintr(type, thread_intr);

  handlers[type] = handler;  /* OS側から呼び出す割込みハンドラを登録 */
  putcurrent();

  return 0;
}

static void call_functions(kz_syscall_type_t type, kz_syscall_param_t *p)
{
  /* システムコールの実行中にcurrentが書き換わるので注意 */
  switch(type) {
    case KZ_SYSCALL_TYPE_RUN: /* kz_run() */
      p->un.run.ret = thread_run(p->un.run.func, p->un.run.name,
                                 p->un.run.priority, p->un.run.stacksize,
                                 p->un.run.argc, p->un.run.argv);
      break;
    case KZ_SYSCALL_TYPE_EXIT:    /* kz_exit() */
      /* TCBが消去されるので、戻り値を書き込んではいけない */
      thread_exit();
      break;
    case KZ_SYSCALL_TYPE_WAIT:    /* kz_wait() */
      p->un.wait.ret = thread_wait();
      break;
    case KZ_SYSCALL_TYPE_SLEEP:   /* kz_sleep() */
      p->un.sleep.ret = thread_sleep();
      break;
    case KZ_SYSCALL_TYPE_WAKEUP:  /* kz_wakup() */
      p->un.wakeup.ret = thread_wakeup(p->un.wakeup.id);
      break;
    case KZ_SYSCALL_TYPE_GETID:   /* kz_getid() */
      p->un.getid.ret = thread_getid();
      break;
    case KZ_SYSCALL_TYPE_CHPRI:   /* kz_chpri() */
      p->un.chpri.ret = thread_chpri(p->un.chpri.priority);
      break;
    case KZ_SYSCALL_TYPE_KMALLOC:   /* kz_kmalloc() */
      p->un.kmalloc.ret = thread_kmalloc(p->un.kmalloc.size);
      break;
    case KZ_SYSCALL_TYPE_KMFREE:   /* kz_kmfree() */
      p->un.kmfree.ret = thread_kmfree(p->un.kmfree.p);
      break;
    case KZ_SYSCALL_TYPE_SEND:   /* kz_send() */
      p->un.send.ret = thread_send(p->un.send.id,
                                  p->un.send.size, p->un.send.p);
      break;
    case KZ_SYSCALL_TYPE_RECV:   /* kz_recv() */
      p->un.recv.ret = thread_recv(p->un.recv.id,
                                  p->un.recv.sizep, p->un.recv.pp);
      break;
    case KZ_SYSCALL_TYPE_SETINTR:   /* kz_setintr() */
      p->un.setintr.ret = thread_setintr(p->un.setintr.type, p->un.setintr.handler);
      break;
    default:
      break;
  }
}

/* システムコールの処理 */
static void syscall_proc(kz_syscall_type_t type, kz_syscall_param_t *p)
{
  /*
   * システムコールを呼び出したスレッドをレディキューから
   * 外した状態で処理関数を呼び出す。このためシステムコールを
   * 呼び出したスレッドをそのまま動作継続させたい場合には
   * 処理関数の内部で putcurrent() を行う必要がある。
   */
   getcurrent();
   call_functions(type, p);
}

/* サービスコールの処理 */
static void srvcall_proc(kz_syscall_type_t type, kz_syscall_param_t *p)
{
  /*
   * システムコールとサービスコールの処理関数の内部で
   * システムコールの実行したスレッドIDを得るために current を
   * 参照している部分があり（たとえば thread_send() など）、
   * current が残っていると誤動作するため NULL に設定する。
   * サービスコールは thread_intrvec() 内部の割り込みハンドラ呼び出しの
   * 延長で呼ばれているはずなので、呼び出し後に thread_intrvec() で
   * スケジューリング処理が行われ、current は再設定される。
   */
  current = NULL;
  call_functions(type, p);
}

/* スレッドのスケジューリング */
static void schedule(void)
{
  int i;

  /*
   * 優先順位の高い順（湯煎度の数値の小さい順）にレディキューを見て
   * 動作可能なスレッドを検索する
   */
  for (i = 0; i < PRIORITY_NUM; i++) {
    if (readyque[i].head) /* 見つかった */
      break;
  }
  if (i == PRIORITY_NUM) /* 見つからなかった */
    kz_sysdown();

  current = readyque[i].head;  /* カレントスレッドに設定する */
}

static void syscall_intr(void)
{
  syscall_proc(current->syscall.type, current->syscall.param);
}

static void softerr_intr(void)
{
  puts(current->name);
  puts(" DOWN.\n");
  getcurrent();   /* レディキューから外す */
  thread_exit();  /*スレッドを終了する */
}

/* 割込み処理の入り口関数 */
static void thread_intr(softvec_type_t type, unsigned long sp)
{
  /* カレントスレッドのコンテキストを保存する */
  current->context.sp = sp;

  /*
   * 割込みごとの処理を実行する
   * SOFTVEC_TYPE_SYSCALL, SOFTVEC_TYPE_SOFTERRの場合は
   * syscall_intr(), softerr_intr() がハンドラに登録されているので
   * それらが実行される。
   * それ以外の場合は、kz_setintr() によってユーザ登録されたアンドラが
   * 実行される。
   */
  if (handlers[type])
    handlers[type]();

  schedule();   /* スレッドのスケジューリング */

  /*
   * スレッドのディスパッチ
   * (dispatchc)()関数の本体は startup.s にあり、アセンブラで記述されている
   */
  dispatch(&current->context);
  /* ここには返ってこない */
}

void kz_start(kz_func_t func, char *name, int priority, int stacksize,
              int argc, char *argv[])
{
  kzmem_init(); /* 動的メモリの初期化 */

  /*
   * 以降で呼び出すスレッド関連のライブラリ関数の内部で current を
   * 見ている場合があるので current を NULL に初期化しておく
   */
  current = NULL;

  memset(readyque, 0, sizeof(readyque));
  memset(threads,  0, sizeof(threads));
  memset(handlers, 0, sizeof(handlers));
  memset(msgboxes, 0, sizeof(msgboxes));

  /* 割込みハンドラの登録 */
  thread_setintr(SOFTVEC_TYPE_SYSCALL, syscall_intr);  /* システムコール */
  thread_setintr(SOFTVEC_TYPE_SOFTERR, softerr_intr);  /* ダウン要因発生 */

  /* システムコール発行不可なので直接関数を呼び出してスレッドを作成する */
  current = (kz_thread *)thread_run(func, name, priority, stacksize, argc, argv);

  /* 最初のスレッドを起動 */
  dispatch(&current->context);

  /* ここには返ってこない */
}

void kz_sysdown(void)
{
  puts("system error!\n");
  while (1)
    ;
}

/* システムコール呼び出し用ライブラリ関数 */
void kz_syscall(kz_syscall_type_t type, kz_syscall_param_t *param)
{
  current->syscall.type  = type;
  current->syscall.param = param;
  asm volatile ("trapa #0");  /* トラップ割込み発行 */
}

/* サービスコール呼び出し用ライブラリ関数 */
void kz_srvcall(kz_syscall_type_t type, kz_syscall_param_t *param)
{
  srvcall_proc(type, param);
}
