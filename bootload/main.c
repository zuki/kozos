#include "defines.h"
#include "serial.h"
#include "lib.h"

int main(void)
{
  serial_init(SERIAL_DEFAULT_DEVICE); /* シリアル・デバイスの初期化 */

  puts("Hello world!\n");

  putxval(0x10, 0);   puts("\n");
  putxval(0xffff, 0); puts("\n");

  while(1)                            /* 停止 */
    ;

  return 0;
}
