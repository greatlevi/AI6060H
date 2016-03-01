#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <drv_uart.h>

#define CONSOLE_UART	SSV6XXX_UART0
#define DATA_UART		SSV6XXX_UART1

#define UART_BUF_MAX_SIZE (256)
#define UART_FW_RX_UNIT (16)   //size in each transfer!!
#define UART_FIFO_SIZE (16)

#define UART_CHK_SUM_RETRANSMIT (0xffff << 3)   //when this value being received,it means retransmit being made!!

#define UART_DBG_FLASH_ADDRESS  (0x60000)       //384kb

typedef enum t_EVENTMSG_ID 
{
	/* Message definition for ATE process */
	MSG_ATE_RXRECV				= 0x00,
	MSG_ATE_SCAN,
	MSG_ATE_DISCONNECT,
	MSG_ATE_RECV,
	MSG_ATE_ERROR,

	/* Message definition for ATE process */
	MSG_AUTO_REBOOT,
	MSG_AUTO_GETCONFIGURATION,
	MSG_AUTO_SETCONFIGURATION,
	MSG_AUTO_GETNETSTATUS,
	MSG_AUTO_SCAN,
	MSG_AUTO_CONNECT,
	MSG_AUTO_SEND,
	MSG_AUTO_DISCONNECT,

	MSG_EV_GET_IP
} EVENTMSG_ID;

typedef struct t_EVENTMSG_DATA
{
	EVENTMSG_ID		msgid;
	int			    data_len;
	unsigned char	data[UART_BUF_MAX_SIZE];
	unsigned char	used;
} EVENTMSG_DATA ;


extern int uart_bin_mode;
extern int uart_fw_size;
extern unsigned int flash_address;   //address:384KB   
extern unsigned int flash_write_ptr;


void drv_rx_pointer_rewind() ATTRIBUTE_SECTION_SRAM;
void drv_rx_bin(void *m_data) ATTRIBUTE_SECTION_SRAM;
void drv_rx_txt(void *m_data) ATTRIBUTE_SECTION_SRAM;
void cosnole_rx_handler(void *m_data) ATTRIBUTE_SECTION_SRAM;

int console_init(void *m_data); //init only. It's ok to put it in flash!


#endif /* _CONSOLE_H_ */
