#include "main.h"
#include <sys/attribs.h>
#include <xc.h>

int         main(void)
{
    LATFbits.LATF1 = SET_LED_OFF;
    TRISFbits.TRISF1 = PIN_OUTPUT_MODE;
    
    IEC0_INT1_BIT;
    //IEC0bits.INT1IE = 1;

    while (1) continue ;
    
    return (0);
}

void        __ISR(7, IPLSOFT) testf(void)
{
    LATFbits.LATF1 = SET_LED_ON;
}

/*

void
led_init(void)
{
    LATFbits.LATF1 = SET_LED_OFF;
    TRISFbits.TRISF1 = PIN_OUTPUT_MODE;
}

u8
led_process(void)
{
    static u8 btn_state = DEFAULT_BTN_STATE;

    if (BTN_STATE)
    {
        btn_state = btn_state | PUSHED_BTN_STATE;
        if (btn_state == BTN_FULL_CYCLE)
        {
            INVERT_LED_VALUE;
            btn_state = RESET_BTN_STATE;
            return (1);
        }
    }
    else
        btn_state = btn_state | RELEASED_BTN_STATE;
    return (0);
}

void
led_blink_mode(void)
{
    u16         *time_value;

    time_value = get_time_value();
    if (TMR1 == *time_value)
    {
        TMR1 = 0x00;
        INVERT_LED_VALUE;
    }
}

u8
led_pushtimer(void)
{
    static u8   btn_state = DEFAULT_BTN_STATE;
    static u8   divide = 1;

    if (TMR1 == (TIME_CONST / divide))
    {
        TMR1 = 0x00;
        INVERT_LED_VALUE;
    }
    if (BTN_STATE)
    {
        btn_state = btn_state | PUSHED_BTN_STATE;
        if (btn_state == BTN_FULL_CYCLE)
        {
            divide *= 2;
            if (!(divide % 32))
                divide = 1;
            btn_state = RESET_BTN_STATE;
            return (1);
        }
    }
    else
        btn_state = btn_state | RELEASED_BTN_STATE;
    return (0);
}

void
led_turn_off(void)
{
    LATFbits.LATF1 = 0x00;
}

void
led_turn_on(void)
{
    LATFbits.LATF1 = 0x01;
}

u8
get_led_state(void)
{
    return (LATFbits.LATF1);
}

*/