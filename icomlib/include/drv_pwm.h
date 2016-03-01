#ifndef __DRV_PWM_H__
#define __DRV_PWM_H__

#define PWM_LEVEL 10

void drv_pwm_init();
void drv_pwm_setduty(U32 dutycycle);
void drv_pwm_enable();
void drv_pwm_deinit();
void drv_pwm_disable();

#endif

