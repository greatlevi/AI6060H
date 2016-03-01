#ifndef _GPIO_API_H_
#define _GPIO_API_H_

#include <drv_gpio.h>

typedef struct t_GPIO_CONF
{
	GPIO_DIRECTION 	dirt;       //GPIO direction. INPUT or OUTPUT
	BOOL 			pull_en;    //Enable pull-down resistor. 0:diable, 1:enable.
	BOOL			driving_en; //output with stronger driving current. 0:diable, 1:enable.
}GPIO_CONF;

/**
 * @brief Configures the specified pin to behave either as an input or an output. 
 * @param id		[IN] one of enum type of GPIO_ID.
 * @param conf	    [IN] GPIO configure. Please refer struct GPIO_CONF for the detail.
 * @return the result. 0 : Successful, -1 : Failed.
 */
S32 pinMode(GPIO_ID id, GPIO_CONF conf);
/**
 * @brief Set the output level of the pin. It's only work when configures the pin to behave as an output.
 * @param id		[IN] one of enum type of GPIO_ID.
 * @param data	    [IN] 0 : output low. 1 : output high.
 * @return the result. 0 : Successful, -1 : Failed.
 */
S32 digitalWrite(GPIO_ID id, U32 data);
/**
 * @brief Reads the value from a specified pin. 
 * @param id		[IN] one of enum type of GPIO_ID.
 * @return the value from the pin. It's only reliable when configures the pin to behave as an Input.
 */
S32 digitalRead(GPIO_ID id);
/**
 * @brief Renable GPIO interrupt with a specified pin. 
 * @param id		 [IN] one of enum type of GPIO_ID.
 * @param mode		 [IN] trigger mode. one of enum type of GPIO_TRIGGER_MODE.
 * @param callbackfn [IN] callback function when GPIO interrupt is triggered.
 * @param data		 [IN] the pointer of data pass to callback function.
 * @return the result. 0 : Successful, -1 : Failed.
 */
S32 gpio_irq_enable(GPIO_ID id, GPIO_TRIGGER_MODE mode, void (*callbackfn)(void *), void *data);
/**
 * @brief disable GPIO interrupt. 
 * @return none.
 */
void gpio_irq_disable();


#endif /* _GPIO_API_H_ */
