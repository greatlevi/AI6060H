#ifndef _DRV_GPIO_H_
#define _DRV_GPIO_H_

typedef enum t_GPIO_DIRECTION
{
	INPUT 		= 0,
	OUTPUT,
} GPIO_DIRECTION;

typedef enum t_GPIO_ID
{
	GPIO_1 		= 0,
	GPIO_2,
	GPIO_3,
    GPIO_5,
    GPIO_6,
	GPIO_8,
	GPIO_15,
	GPIO_18,
	GPIO_19,
	GPIO_20,
	GPIO_MAX
} GPIO_ID;


typedef enum t_GPIO_TRIGGER_MODE
{
	HIGH_LEVEL 	= 0,
	LOW_LEVEL,
	RISING_EDGE,
	FALLING_EDGE,
} GPIO_TRIGGER_MODE;

S32 drv_gpio_Output(GPIO_ID id, BOOL pullen, BOOL driven);
S32 drv_gpio_Input(GPIO_ID id, BOOL pullen);
S32 drv_gpio_write(GPIO_ID id, U32 data);
S32 drv_gpio_read(GPIO_ID id);
S32 drv_gpio_set_irq(GPIO_ID id, GPIO_TRIGGER_MODE mode);

#endif /* _DRV_GPIO_H_ */
