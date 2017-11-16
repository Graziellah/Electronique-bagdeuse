#include "main.h"

// Sebby

// Anthony

void
timer1_init(void)
{
    T1CONbits.TWIP = 0x00; // Asynchronous Timer Write in Progress bit
    T1CONbits.SIDL = 0x00; // Stop in Idle Mode bit (0 = Dont stop in idle mode)
    T1CONbits.TCKPS = 0x03; // Timer Input Clock Prescale Select bits (3 = 1:256)
    T1CONbits.TSYNC = 0x00; // Timer External Clock Input Synchronisation Selection bit (0 = External clock not sync)
    T1CONbits.TCS = 0x00; // Timer Clock Source Select bit (0 = Internal peripheral clock)
    T1CONbits.TGATE = 0x00;

    TMR1 = 0x00; // Clear Timer Register
    PR1 = 0xFFFF; // Load Period Register

    T1CONbits.ON = 0x01; // Timer On bit (1 = Start the timer)
}

void
timer2_init(void)
{
    T2CONbits.SIDL = 0x00; // Stop in Idle Mode bit (0 = Dont stop in idle mode)
    T2CONbits.TCKPS = 0x03; // Timer Input Clock Prescale Select bits (3 = 1:256)
    T2CONbits.TGATE = 0x00;

    TMR2 = 0x00; // Clear Timer Register
    PR2 = 0xFFFF; // Load Period Register

    T2CONbits.ON = 0x01; // Timer On bit (1 = Start the timer)
}

u16*
get_time_value(void)
{
    static u16 time_value = TIME_CONST;

    return (&time_value);
}

void
set_time_value(u16 value)
{
    u16         *time_value;

    time_value = get_time_value();
    *time_value = value;
}
