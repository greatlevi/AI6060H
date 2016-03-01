#ifndef _SERIAL_API_H_
#define _SERIAL_API_H_

#include <drv_uart.h>
#include <ssv_types.h>
/**
 * @brief Enable serial bus. 
 * @param rate			[IN] The baud rate of the serial bus.
 * @param callbackfn	[IN] callback function pointer.
 * @param data	      	[IN] The argument pointer of the callback function.
 * @return none.
 */
void SerialInit(BAUD_RATE rate, void (*callbackfn)(void *), void *data);
/**
 * @brief Disable serial bus.
 * @return none.
 */
void SerialDeinit(void);
/**
 * @brief Configure serial bus, the default setting is SERIAL_8N1. 
 * @param rate[IN] Serial bus configuration. SERIAL_8N1 : data length 8 bits, parity none, 1 stop bit.
 * @return none.
 */
void SerialConfig(SERIAL_CONFIG conf);
/**
 * @brief Output the data to tx. 
 * @param outdata [IN] The buffer of output data.
 * @param size	 [IN] The size of the buffer.
 * @return none.
 */
void SerialWrite(U8 *outdata, U32 size) ATTRIBUTE_SECTION_SRAM;
/**
 * @brief Read the data get from rx. 
 * @return the data get from rx. -1: no data is available.
 */
S32 SerialRead(void) ATTRIBUTE_SECTION_SRAM;

#endif
