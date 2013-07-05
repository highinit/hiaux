#include "hword_master.h"

void *start_write_thread(void *a)
{
    HwordMaster *hword_master = (HwordMaster*)a;
    hword_master->writeAsyncThread();
}
