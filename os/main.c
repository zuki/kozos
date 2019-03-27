#include "defines.h"
#include "serial.h"
#include "lib.h"

int main(void)
{
  static char buf[32];

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
