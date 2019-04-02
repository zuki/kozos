#include "kozos.h"
#include "thread.h"
#if 0
#include "stublib.h"
#endif

int mainfunc(int argc, char *argv[])
{
#if 0
  kz_debug(stub_proc);
#endif

  extintr_id  = kz_run(extintr_main,  "extintr",   1, 0, NULL);
  idle_id     = kz_run(idle_main,     "idle",     31, 0, NULL);
#if 0
  command0_id = kz_run(command_main,  "command0", 11, 0, NULL);
#endif
  command1_id = kz_run(command_main,  "command1", 11, 1, NULL);

  return 0;
}

int kozos_start(int argc, char *argv[])
{
  kz_start(mainfunc, "main", 0, argc, argv);
  return 0;
}
