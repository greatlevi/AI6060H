#ifndef _DRV_PMU_H_
#define _DRV_PMU_H_

#include <ssv6200_reg.h>//#include <regs.h>
#include <ssv6200_common.h>
//#include <ssv_printf.h>
#include <ssv_types.h>
#ifdef __PMU_VERIFY__
#include "rtc/drv_rtc.h"
#include "log.h"
#endif  /*__PMU_VERIFY__ */

#define WAKEUP_EVENT_TIMER      1
#define WAKEUP_EVENT_INTERRUPT  2
//------------------------------------------------------------------------------
//
//  Function:   drv_pmu_setwake_cnt 
//
//  Description
//      Set PMU sleep wake cnt
//
//  Parameters
//      wake_cnt: time to sleep, count by 32K RC clock.
//
//  Return Value
//      0 indicates success.
//      -1 indicates failure.
//
//------------------------------------------------------------------------------
S32 drv_pmu_setwake_cnt (U32 t_ms);

//------------------------------------------------------------------------------
//
//  Function:   drv_pmu_sleep
//
//  Description
//      Set PMU go into sleep state
//
//  Parameters
//
//  Return Value
//      0 indicates success.
//      -1 indicates failure.
//
//------------------------------------------------------------------------------
S32 drv_pmu_sleep (void);

//------------------------------------------------------------------------------
//
//  Function:   drv_pmu_init
//
//  Description
//      Prepare works before PMU go to sleep mode, including:
//      1. Set DIGI_TOP_POR_MASK
//
//  Parameters
//
//  Return Value
//
//------------------------------------------------------------------------------
void drv_pmu_init(void);

//------------------------------------------------------------------------------
//
//  Function:   drv_pmu_chk
//
//  Description
//      Check the reason PMU go to ON state, and clear DIGI_TOP_POR_MASK
//
//  Parameters
//
//  Return Value
//
//------------------------------------------------------------------------------
void drv_pmu_chk (void);
U8 drv_pmu_check_interupt_event(void);
void drv_pmu_powersave_mode() ATTRIBUTE_SECTION_SRAM;
void drv_pmu_normal_mode(U8 ch) ATTRIBUTE_SECTION_SRAM;
void drv_reset_pstate_txsend() ATTRIBUTE_SECTION_SRAM;  //to-do: check if to put it into flash
void drv_reset_pstate_all() ATTRIBUTE_SECTION_SRAM; //to-do: check if to put it into flash
#endif /* _DRV_PMU_H_ */

