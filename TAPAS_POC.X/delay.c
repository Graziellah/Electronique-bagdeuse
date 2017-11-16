#include "delay.h"

void delay(u32 time_ms)
{
	g_time_count = 0;	//init counter
	TMR2 = 0;
	T2CONbits.ON = 1;
	IEC0bits.T2IE = 1;
	while(g_time_count < time_ms)
	{
		//IEC0bits.T2IE = 1;
	}
	T2CONbits.ON = 0;
	IEC0bits.T2IE = 0;
}

void __ISR(_TIMER_2_VECTOR, IPL7AUTO) delay_int(void)
{
	//IEC0bits.T2IE = 0;
	IFS0bits.T2IF = 0;
	//TMR2 = 0;
	g_time_count++;
}
