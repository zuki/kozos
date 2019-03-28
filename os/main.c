#include "defines.h"
#include "serial.h"
#include "lib.h"

#if 1
static int init(void)
{
  extern int bss_start, ebss;

  /* clear BSS */
  memset(&bss_start, 0, (long)&ebss - (long)&bss_start);

  serial_init(SERIAL_DEFAULT_DEVICE);

  return 0;
}
#endif

int main(void)
{
  static char buf[32];

  init();

  puts("Hello World!\n");

  while(1) {
    puts("> "); /* プロンプト表示 */
    gets(buf);        /* シリアルからのコマンド受信 */

    if (!strncmp(buf, "echo", 4)) {
      puts(buf + 4);
      puts("\n");
    } else if (!strcmp(buf, "exit")) {
      puts("exit\n");
      break;
    } else {
      puts("unknown.\n");
    }
  }

  return 0;
}
