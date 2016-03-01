#ifndef _DRV_FENCE_H_
#define _DRV_FENCE_H_

#include <ssv_lib.h>
#include "ssv_regs.h"
#include "ssv6200_aux.h"
#include <irq.h>

void hwfence_init();
int hwfence_start(irq_handler fence_handle,void *m_data);


#endif
