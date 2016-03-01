#ifndef __DEBUG_UART_H__CABRIO_E__
#define __DEBUG_UART_H__CABRIO_E__

void
dbg_set_input_handler(void (*handler)(const char *inp, unsigned int len));

unsigned int
dbg_send_bytes(const unsigned char *seq, unsigned int len);

#if 0
void
dbg_printf(const char *format, ...)
     __attribute__ ((__format__ (__printf__, 1,2)));
     
void
dbg_blocking_printf(const char *format, ...)
     __attribute__ ((__format__ (__printf__, 1,2)));
#endif
     
void
dbg_putchar(const char ch);

void
dbg_blocking_putchar(const char ch);

#endif /* __DEBUG_UART_H__CABRIO_E__ */
