#include "main.h"
#include <sys/attribs.h>
#include <xc.h>

int         main(void)
{
    // Setup LED
    LATFbits.LATF1 = LED_STATE_OFF;
    TRISFbits.TRISF1 = PIN_OUTPUT_MODE;


    INTCONbits.MVEC = 1; // Enable multi vector mode

    T2CONbits.ON = 0; // Stop the timer
    TMR2 = 0x0;         // Clear the timer register
    PR2 = 65535;        // Load the period register

    IPC2bits.T2IP = 4; // Set priority level = 4
    IFS0bits.T2IF = 0; // Clear the timer interrupt status flag
    IEC0bits.T2IE = 1; // Enable timer interrupts

    T2CONbits.TCKPS = 0x3; // Prescaler 1:256

    T2CONbits.ON = 1; // Start the timer p

    __builtin_enable_interrupts();

    while (1) {;}

    return (0);
}

void        __ISR(_TIMER_2_VECTOR, IPL4) timerf(void)
{
    INVERT_LED_VALUE;
    IFS0bits.T2IF = 0; // Clear the timer flag
}
