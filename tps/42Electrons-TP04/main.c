
#include <sys/attribs.h>
#include <plib.h> // Microchip PIC32MX Peripheral Library
#include <stdint.h>

//Defines

#define CLOCK 32768 //Normaly Timer1 is 32,768KHz
#define CLOCK2 1000000L //Normaly Timer2 is 1MHz (Primary Oscilator with a 1:8 prescaler)
#define INPUT 1
#define OUTPUT 0
#define HIGH 1
#define LOW 0
#define INVERT_LED (LATFINV = 0b10)
#define LED_MASK 0x00000002
#define LED_OFF (LATFCLR = LED_MASK) // (LATF1) - Don't know if this will work
#define LED_ON (LATFSET = LED_MASK) // (LATF1) - This one either
#define BUTTON_PRESSED (PORTD)

// Typedefs
typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long int u32;

typedef char s8;
typedef int s16;
typedef long int s32;

u8 button = 0;
u8 mode_index = 0;
u8 mode[] = {1, 2, 4, 8, 16};
u8 pwm_ramp = 1;
float value = 0.0f; // Min 0.01 ; Max 1
int count = 0; // Max 10 ?
int duty_cycle = 0; // Min 1 (PR2 100) ; Max 100 (PR2 10 000)
int max_pr2 = 10000;

int main()
{
    //Oscilator config
    OSCCONbits.COSC = 0b010;
    /*
    111 = Internal Fast RC (FRC) Oscillator divided by OSCCON<FRCDIV> bits
    110 = Internal Fast RC (FRC) Oscillator divided by 16
    101 = Internal Low-Power RC (LPRC) Oscillator
    100 = Secondary Oscillator (SOSC)
    011 = Primary Oscillator (POSC) with PLL module (XTPLL, HSPLL or ECPLL)
    010 = Primary Oscillator (POSC) (XT, HS or EC)
    001 = Internal Fast RC Oscillator with PLL module via Postscaler (FRCPLL)
    000 = Internal Fast RC (FRC) Oscillator
    */

    //Timer1
    T1CONbits.TCKPS = 0; //Prescaler
    TMR1 = 0;
    PR1 = CLOCK; //Load period register
    T1CONbits.ON = 1; // start timer 1
    T1CONbits.TCS = 1;

    //Timer2
    T2CONbits.ON = 0;   // Stop the timer
    TMR2 = 0b0;         // Clear the timer register
    PR2 = 10000;        // Load the period register
    T2CONbits.TCKPS = 0b00; //(0b mean bit, we are writing 01 in the TCKPS field of the T2CONbits register)
    /*
        11 = 1:256 prescale value
        10 = 1:64 prescale value
        01 = 1:8 prescale value
        00 = 1:1 prescale value
    */
    T2CONbits.ON = 1; // start timer 2

    //LED and Button
    LATFbits.LATF1 = LOW; //Set led to LOW before setting it as an output
    TRISFbits.TRISF1 = OUTPUT; //LED
    TRISDbits.TRISD8 = INPUT; //Button

    //ISR Timer 1
    IFS0bits.T1IF = 0; //Unset the flag
    IEC0bits.T1IE = 1; //Enable the Timer 1 interrupt
    IPC1bits.T1IP = 3; //Set the priority to 3

    //ISR Timer 2
    IFS0bits.T2IF = 0; //Unset the flag
    IEC0bits.T2IE = 0; //Disable the Timer 2 interrupt
    IPC2bits.T2IP = 2; //Set the priority to 2

    //ISR button
    IFS0bits.INT1IF = 0; //Unset the flag
    IEC0bits.INT1IE = 1; //Enable the button interrupt
    IPC1bits.INT1IP = 1; //Set the priority to 7

    //Multi vector interrupt
    INTCONbits.MVEC = 1;

    //Enable interrupts
    __builtin_enable_interrupts();

    while (1)
    {
        WDTCONbits.WDTCLR = 1; //Pet the dog
    }
    return (0);
}

void __ISR(_TIMER_2_VECTOR, IPL2AUTO) Timer2Handler(void)
{
    
    value = 0.0f;
    count += 1;
    if (PORTFbits.RF1 == 0)
    {
        value = (float)duty_cycle / 100;
        LED_ON;
        PR2 = (max_pr2 * value) ? (max_pr2 * value) : 1;
    }
    else
    {
        LED_OFF;
        PR2 = (max_pr2 - PR2) ? (max_pr2 - PR2) : 1;

        if (duty_cycle >= 100)
            pwm_ramp = 0;
        else if (duty_cycle <= 0)
            pwm_ramp = 1;

        if (pwm_ramp && count >= 5)
        {
            duty_cycle += 1;
            count = 1;
        }
        else if (!pwm_ramp && count >= 5)
        {
            duty_cycle -= 1;
            count = 1;
        }
    }
    IFS0bits.T2IF = 0;
    TMR2 = 0;
}

void __ISR(_TIMER_1_VECTOR, IPL3AUTO) Timer1Handler(void)
{
    PR1 = CLOCK / mode[mode_index];
    INVERT_LED;
    if (button)
    {
        LED_OFF;
        IEC0bits.T1IE = 0; //Disable the Timer 1 interrupt
        IEC0bits.T2IE = 1; //Enable the Timer 2 interrupt
    }
    IFS0bits.T1IF = 0;
}

void __ISR(_EXTERNAL_1_VECTOR, IPL1AUTO) ButtonHandler(void)
{
    IEC0bits.T1IE = 1; //Enable the Timer 1 interrupt
    IEC0bits.T2IE = 0; //Disable the Timer 2 interrupt
    IFS0bits.T2IF = 0; //Unset the Timer 2 flag

    TMR1 = 0; //Reset TMR0 register
    PR1 = 65535; //So the period is 2sec

    while (PORTDbits.RD8 == 0) button = 1;
    button = 0;
    mode_index = (mode_index >= 4) ? 0 : mode_index + 1;

    IFS0bits.INT1IF = 0;
}
