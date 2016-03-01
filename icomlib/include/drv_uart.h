#ifndef _DRV_UART_H_
#define _DRV_UART_H_

#include <ssv_types.h>

typedef enum __SSV6XXX_UART_ID_E {
    SSV6XXX_UART0,
    SSV6XXX_UART1
} SSV6XXX_UART_ID_E;

typedef enum t_BAUD_RATE
{
	BAUD_1200 		= 0,
	BAUD_2400,
	BAUD_4800,
	BAUD_9600,
	BAUD_14400,
	BAUD_19200,
	BAUD_38400,
	BAUD_57600,
	BAUD_115200,
	BAUD_230400,
	BAUD_460800,
	BAUD_921600,
} BAUD_RATE;

typedef enum t_SERIAL_CONFIG
{
	SERIAL_5N1 		= 0,
	SERIAL_6N1,
	SERIAL_7N1,
	SERIAL_8N1, //default setting
	SERIAL_5N2,
	SERIAL_6N2,
	SERIAL_7N2,
	SERIAL_8N2,
	SERIAL_5E1,
	SERIAL_6E1,
	SERIAL_7E1,
	SERIAL_8E1,
	SERIAL_5E2,
	SERIAL_6E2,
	SERIAL_7E2,
	SERIAL_8E2,
	SERIAL_5O1,
	SERIAL_6O1,
	SERIAL_7O1,
	SERIAL_8O1,
	SERIAL_5O2,
	SERIAL_6O2,
	SERIAL_7O2,
	SERIAL_8O2,
} SERIAL_CONFIG;

S32 drv_uart_rx_ready(SSV6XXX_UART_ID_E	uart_id) ATTRIBUTE_SECTION_SRAM;
S32 drv_uart_rx(SSV6XXX_UART_ID_E uart_id) ATTRIBUTE_SECTION_SRAM;
S32 drv_uart_tx(SSV6XXX_UART_ID_E uart_id, S32 ch) ATTRIBUTE_SECTION_SRAM;
//u8 drv_uart_tx(SSV6XXX_UART_ID_E uart_id, u8 ch);

S32 drv_uart_init(SSV6XXX_UART_ID_E uart_id);

S32 drv_uart_rx_0(void) ATTRIBUTE_SECTION_SRAM;
S32 drv_uart_tx_0(S8 ch);
S32 drv_uart_is_tx_busy_0(void);
S32 drv_uart_multiple_tx_0(int num, const S8 *pch);
S32 drv_uart_get_uart_fifo_length_0(void);

void drv_uart_enable_tx_int_0(void);
void drv_uart_disable_tx_int_0(void);
void drv_uart_enable_rx_int_0(void);
void drv_uart_disable_rx_int_0(void);
void drv_datauart_init(BAUD_RATE rate);
void drv_datauart_Deinit();
void drv_datauart_conf(SERIAL_CONFIG conf);
void drv_uart_40M() ATTRIBUTE_SECTION_SRAM;
void drv_uart_26M() ATTRIBUTE_SECTION_SRAM;

#endif /* _DRV_UART_H_ */
