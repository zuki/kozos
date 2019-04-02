#include "lib.h"
#include "interrupt.h"

int interrupt_init()
{
  int vec;
  for (vec = 0; vec < VECTOR_NUM; vec++)
    interrupt_sethandler(vec, NULL);
  return 0;
}

int interrupt_sethandler(int vec, interrupt_handler_t handler)
{
  VECTORS[vec] = handler;
  return 0;
}

void interrupt(int vec)
{
  interrupt_handler_t handler = VECTORS[vec];
  if (handler)
    handler(vec);
  return;
}
