#include "defines.h"
#include "serial.h"
#include "lib.h"

int main(void)
{
  serial_init(SERIAL_DEFAULT_DEVICE); /* シリアル・デバイスの初期化 */

  puts("Hello world!\n");

  while(1)                            /* 停止 */
    ;

  return 0;
}
