#include "uart.h"

void uart2_init(void)
{
	U2STAbits.URXEN = 0;				// Reciver Enable bit
	U2STAbits.UTXEN = 1;				// Transmit Enable bit
	U2BRG = ((SYSCLK)/16/(BAUDRATE)-1);	// Baudrate
	U2MODEbits.ON = 1;					// UART Enable bit
}

void uart2_put_char(u8 c)
{
	while (U2STAbits.UTXBF);
	U2TXREG = c;
}

void uart2_put_str(u8* str)
{
	while(*str)
	{
		uart2_put_char(*str++);
	}
}

void uart2_put_byte(u8 data)
{
	u8 i = 8;
	while(U2STAbits.UTXBF);
	while(i)
	{
		uart2_put_char(((data >> i - 1) & 0b1) + 48);
		i--;
	}
}
