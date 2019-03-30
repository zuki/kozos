#include "defines.h"
#include "kozos.h"
#include "interrupt.h"
#include "lib.h"

/* システムタスクとユーザスレッドの起動 */
static int start_threads(int argc, char *argv[])
{
  kz_run(test08_1_main, "command", 0x100, 0, NULL);
  return 0;
}

int main(void)
{
  INTR_DISABLE;   /* 割込みを無効にする */

  puts("kozos boot succeed!\n");

  /* OSの動作開始 */
  kz_start(start_threads, "start", 0x100, 0, NULL);
  /* ここには返ってこない */

  return 0;
}
