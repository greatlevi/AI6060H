#ifndef _DRV_WDOG_H
#define _DRV_WDOG_H

#include <ssv_types.h>

void drv_wdog_reboot();
void drv_wdog_period_5000ms();
void drv_wdog_period_10000ms();
void drv_wdog_period_cutommed(S32 wtimer_in_ms);
void drv_wdog_stop();

#endif  //_DRV_WDOG_H
