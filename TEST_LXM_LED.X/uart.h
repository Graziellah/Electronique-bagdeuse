#include "main.h"

#ifndef UART_H
#define UART_H

void uart2_init(void);
void uart2_put_char(u8 c);
void uart2_put_str(u8* str);
void uart2_put_byte(u8 data);

#endif
