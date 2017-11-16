#include "main.h"

// Anthony
u8 blink_mode = 0;

int
main(void)
{
    u8   btn_state = DEFAULT_BTN_STATE;
    u8   divide = 1;

    led_init();
    timer1_init();

    WDT_ON;

    while(1)
    {
        
        if (TMR1 >= (TIME_CONST / divide))
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
                if (divide == 32)
                    divide = 1;
                btn_state = RESET_BTN_STATE;
            }
        }
        else if (btn_state == PUSHED_BTN_STATE)
            btn_state = btn_state | RELEASED_BTN_STATE;
         
        //led_process();
 
        WDT_CLR;
    }
    return (0);
}