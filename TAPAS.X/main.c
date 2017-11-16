#include "main.h"
#include "uart.h"
#include "util.h"
#include "lcd.h"
#include "delay.h"
#include "spi.h"

#define INVERT_LED (LATFINV = 0b10)
#define LED_OFF (LATFbits.LATF1 = 0) //Don't know if this will work
#define LED_ON (LATFbits.LATF1 = 1) //This one either

// Globales tag
u8 g_tag[10] = {0};
u8 g_flag = 0;
u8 g_tag_lrc = 0;
u8 g_nb_bit_read = 0;
u8 g_data_index = 0;

u8 g_buffer[32] = {0};

enum e_mode g_mode = read;

void init_io()
{
	// Button
	TRISDbits.TRISD8 = INPUT;
    
    //LED
	LATFbits.LATF1 = LOW; //Set led to LOW before setting it as an output
	TRISFbits.TRISF1 = OUTPUT; //LED

	// Set DATA_0 & DATA_1 pin as INPUTS
	TRISDbits.TRISD9 = INPUT;
	TRISDbits.TRISD10 = INPUT;
    
    // Flash
    FLASH_CE = HIGH;
    TRISGbits.TRISG9 = OUTPUT; // FLASH CE pin
    FLASH_WP = HIGH;
    TRISDbits.TRISD0 = OUTPUT; // Flash WP pin
    FLASH_HOLD = HIGH;
    TRISDbits.TRISD1 = OUTPUT; // Flash HOLD pin
}

void init_timer()
{
	//T4CONbits.ON = 0;		// Stop timer
	//T4CONCLR = 1;			// Clear timer register
	//PR4 = 8;				// Load period register

	// Timer 2
	T2CON = 0;				// Stop Timer and clear the TXCON register
	PR2 = 1000;				// Load period register
    
    // Timer 3
	T3CON = 0;				// Stop Timer and clear the TXCON register
	PR3 = 65535;			// Load period register
    T3CONbits.TCKPS = 0b11;
    T3CONbits.ON = 0; // Start Timer 3
    
    // Timer 1
    T1CON = 0;
	PR1 = 65535 / 2;              //Load period register
    T1CONbits.TCS = 1;        // TCS: Timer Clock Source Select bit 1 = External clock from TxCKI pin 0 = Internal peripheral clock
	T1CONbits.ON = 0;         // leave timer stopped   
 
}

void init_int()
{
	//Button
	IFS0bits.INT1IF = 0;	//Unset the flag
	IPC1bits.INT1IP = 1;	//Set the priority
	IEC0bits.INT1IE = 1;	//Enable the interruption

	//TIMER 4
	//IFS0bits.T4IF = 0;	//Unset the flag
	//IPC4bits.T4IP = 7;	//Set the priority
	//IEC0bits.T4IE = 1;	//Enable the interruption

	// TIMER 2 used for delay
	IFS0bits.T2IF = 0;		//Unset the flag
	IPC2bits.T2IP = 7;		//Set the priority
	IEC0bits.T2IE = 0;		//Enable the interruption
    
    // TIMER 1
    IFS0bits.T1IF = 0; //Unset the flag
    IPC1bits.T1IP = 4; //Set the
	IEC0bits.T1IE = 1; // enable
	
        
    // TIMER 3
    IFS0bits.T3IF = 0; //Unset the flag
    IPC3bits.T3IP = 4; //Set the priority to 3
	IEC0bits.T3IE = 1; // enable
	

	 //ISR DATA_0
	IFS0bits.INT2IF = 0; //Unset the flag
	IEC0bits.INT2IE = 1; //Enable the button interrupt
	IPC2bits.INT2IP = 3; //Set the priority to 3

	//ISR DATA_1
	IFS0bits.INT3IF = 0; //Unset the flag
	IEC0bits.INT3IE = 1; //Enable the button interrupt
	IPC3bits.INT3IP = 3; //Set the priority to 3

	INTCONbits.MVEC = 1;
	__builtin_enable_interrupts();
}

int main()
{    
    u32 addr;

	SYSTEMConfigPerformance(SYSCLK);

	uart2_init();
	spi2_init();
	init_io();
    init_timer();	
    init_int();
    lcd_init();
    
	while (1)
	{

		if(g_nb_bit_read == 44)
		{
            disable_timout();

			if(g_tag_lrc == lrc(g_tag, 10))
			{
                if (g_mode != read && is_master_tag(g_tag))
                {
                    lcd_clear();
					lcd_put_str("Error");
					lcd_next_line();
                    lcd_put_str("Can't add master");
                    delay(10000);
                    lcd_clear();
                    g_mode = read;
                }

				else if (g_mode == add)
				{
                    if (!search_tag(g_tag, &addr))
                    {
                        if (nb_empty_zone() > 0)
                        {
                            lcd_clear();
                            lcd_put_str("Tag added");
                            lcd_next_line();
                            put_tag(g_tag);
                            delay(10000);
                            lcd_clear();
                        }
                        else
                        {
                            lcd_clear();
                            lcd_put_str("Error:");
                            lcd_next_line();
                            lcd_put_str("Memory is full");
                            delay(10000);
                            lcd_clear();
                        }
                    }
                    else
                    {
                       lcd_clear();
                       lcd_put_str("Error: Tag");
                       lcd_next_line();
                       lcd_put_str("already exist");
                       delay(10000);
                       lcd_clear(); 
                    }
                    
                    g_mode = read;
				}

				else if (g_mode == delete)
				{
					// search in flash
                    // if found - delete
                    //else
                   /// erroe
                    // 
					g_mode = read;
				}

				else if (g_mode == read)
				{
					g_mode = get_tag_mode(g_tag);
					if (g_mode == read)
					{
						// search in flash
                        // if found
                        // green led
                        //relay on
                        // message
                        //else
                        //led red
                        // message
						lcd_clear();
						lcd_put_str("Tag read ");
						lcd_next_line();
						put_tag(g_tag);
					}
					else if (g_mode == add)
					{
						lcd_clear();
						lcd_put_str("Add mode ");
                        enable_timout();
					}
					else if (g_mode == delete)
					{
						lcd_clear();
						lcd_put_str("Delete mode ");
                        enable_timout();
					}
				}
			}

			else
			{
				lcd_clear();
				lcd_put_str("LRC Error");
				g_mode = read;
			}

			g_nb_bit_read = 0;

		}

		WDTCONbits.WDTCLR = 1;	// Clear watchdog
	}
	return (0);
}

void __ISR(_EXTERNAL_1_VECTOR, IPL1AUTO) btn_int(void)
{
	//uart2_put_str("Test 123\r\n");
	//delay(1000);
	//uart2_put_str("delay 123\r\n");
	//uart2_put_byte(0b10101111);
	//uart2_put_str("\r\n");
	//lcd_put_char('a');
    clean_tab(g_buffer, 32);
	lcd_clear();
    spi2_read_Jedec(g_buffer);
    //spi2_read_jedec2();
    lcd_put_str_hex(g_buffer);
    //clean_tab(g_buffer, 32);
	//lcd_put_hex_4bit(0b1111);
	//lcd_display_write_buffer_str("Test", 0, 0);
	//lcd_display_write_buffer_str("Button", 1, 0);
	//lcd_display_update();
	IFS0bits.INT1IF = 0;
}

void __ISR(_EXTERNAL_2_VECTOR, IPL3AUTO) int_data_0(void)
{
	T3CONbits.ON = 1;
    TMR3 = 0;
    
    add_bit(g_tag, &g_data_index, &g_nb_bit_read, &g_tag_lrc, 0);
	//add_bit(0);
	IFS0bits.INT2IF = 0;
}

void __ISR(_EXTERNAL_3_VECTOR, IPL3AUTO) int_data_1(void)
{
	T3CONbits.ON = 1;
    TMR3 = 0;

    add_bit(g_tag, &g_data_index, &g_nb_bit_read, &g_tag_lrc, 1);

    //add_bit(1);
	IFS0bits.INT3IF = 0;
}

void __ISR(_TIMER_1_VECTOR, IPL4AUTO) int_timer_1(void)
{   
    g_timout--;
    
    lcd_put_countdown(g_timout);
    
    if (g_timout == 0)
    {
        lcd_clear();
        T1CONbits.ON = 0; // Stop Timer 1
        g_mode = read;
        g_timout = 9;
    }
    
    INVERT_LED;
    IFS0bits.T1IF = 0; //Unset the flag
 
}

void __ISR(_TIMER_3_VECTOR, IPL4AUTO) int_timer_3(void)
{
    INVERT_LED;
    g_nb_bit_read = 0;
    T3CONbits.ON = 0;
	IFS0bits.T3IF = 0; 
    
    
}