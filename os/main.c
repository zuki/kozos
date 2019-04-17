#include "defines.h"
#include "kozos.h"
#include "interrupt.h"
#include "lib.h"

static int init()
{
  extern int bss_start;
  extern int ebss;

  /* clear BSS */
  memset(&bss_start, 0, (uint32)&ebss - (uint32)&bss_start);

  return 0;
}

/* システムタスクとユーザタスクの起動 */
static int start_threads(int argc, char *argv[])
{
  kz_run(consdrv_main,  "consdrv",  1, 0x100, 0, NULL);
  kz_run(command_main,  "command",  8, 0x100, 0, NULL);
  kz_run(timerdrv_main, "timerdrv", 2, 0x100, 0, NULL);
#if 0
  kz_run(clock_main,    "clock",    9, 0x100, 0, NULL);
#endif
  kz_run(netdrv_main,   "netdrv",  10, 0x100, 0, NULL);
  kz_run(ethernet_main, "ethernet",11, 0x100, 0, NULL);
  kz_run(arp_main,      "arp",     11, 0x100, 0, NULL);
  kz_run(ip_main,       "ip",      12, 0x100, 0, NULL);
  kz_run(icmp_main,     "icmp",    12, 0x100, 0, NULL);
  kz_run(tcp_main,      "tcp",     12, 0x100, 0, NULL);
  kz_run(udp_main,      "udp",     12, 0x100, 0, NULL);
  kz_run(httpd_main,    "httpd",   14, 0x100, 0, NULL);
  kz_run(echo_main,     "echo",    14, 0x100, 0, NULL);

  kz_chpri(15);   /* 優先順位を下げて、アイドルスレッドに移行する */
  INTR_ENABLE;    /* 割込みを有効にする */
  while (1) {
    asm volatile ("sleep");   /* 省電力モードに移行 */
  }

  return 0;
}

int main(void)
{
  init();

  INTR_DISABLE;   /* 割込みを無効にする */

  puts("kozos boot succeed!\n");

  /* OSの動作開始 */
  kz_start(start_threads, "idle", 0, 0x100, 0, NULL);
  /* ここには返ってこない */

  return 0;
}
