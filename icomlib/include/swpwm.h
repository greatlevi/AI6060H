#ifndef __SWPWM_H__
#define __SWPWM_H__

/**
 * @brief Enable SW PWM functionality.  
 * @param id		[IN] one of enum type of GPIO_ID.
 * @param dutycycle	[IN] the duty cycle of the pulse, 0~10. 0 : always low. 10 : always high.
 * @return the result.  -1 : fail. 0 : success.
 */
int enableSwPWM(GPIO_ID id, U8 dutycycle);
/**
 * @brief Disable SW PWM functionality. 
 * @param id		[IN] one of enum type of GPIO_ID.
 * @return none.
 */
void disableSwPWM(GPIO_ID id);

#endif

