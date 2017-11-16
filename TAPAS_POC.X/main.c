#include "main.h"
#include "uart.h"
#include "util.h"
#include "lcd.h"
#include "delay.h"
#include "spi.h"

// Globales tag
u8 g_tag[10] = {0};
u8 g_flag = 0;
u8 g_tag_lrc = 0;
u8 g_nb_bit_read = 0;
u8 g_data_index = 0;

u8 g_timeout = 9;


const u8 g_master_tag_add[10] = {0x6, 0xF, 0x0, 0x0, 0x3, 0x5, 0x3, 0x7, 0x8, 0x7};       // Tag de Sebastien
//const u8 g_master_tag_delete[10] = {0x6, 0xF, 0x0, 0x0, 0x3, 0x5, 0x3, 0x7, 0x8, 0x7};  // Tag de Sebastien
const u8 g_master_tag_delete[10] = {0x6, 0xF, 0x0, 0x0, 0x1, 0xA, 0x4, 0xD, 0x1, 0x6};    // Tag geo add 6 begining
//const u8 g_master_tag_add[10] = {0x0, 0xF, 0x0, 0x0, 0x1, 0xA, 0x4, 0xD, 0x1, 0x6};     // Tag geo add 6 begining

s16 tmp;

u8 g_buffer[32] = {0};
u8 g_page[400][10] = {{0}};

enum e_mode g_mode = read;

void init_io()
{
	// Button
	TRISDbits.TRISD8 = INPUT;
    
    //LED
	LATFbits.LATF1 = LOW; //Set led to LOW before setting it as an output
	TRISFbits.TRISF1 = OUTPUT; //LED
    
    //LED_reader
    LATDbits.LATD2 = LOW; //Set led to LOW before setting it as an output
    TRISDbits.TRISD2 = OUTPUT; //Reader green led

    LATDbits.LATD3 = LOW; //Set led to LOW before setting it as an output
    TRISDbits.TRISD3 = OUTPUT; //Reader red led
    
    //RELAY
    LATDbits.LATD4 = LOW; //Set led to LOW before setting it as an output
    TRISDbits.TRISD4 = OUTPUT; //Relay

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

	//SYSTEMConfigPerformance(SYSCLK);

	uart2_init();
	spi2_init();
	init_io();
    init_timer();	
    init_int();
    lcd_init();

    u8 i = 0;
    
    flash_clear_write_protection();
        
	while (1)
	{
		if(g_nb_bit_read == 44)
		{

                while(i <= 10)
                {
                    uart2_put_char(hextoa(g_tag[i]));
                    if (i < 10)
                    {
                        uart2_put_char(':');
                    }
                    i++;
                }
                uart2_put_str("\r\n");
                i = 0;
                
                disable_timout();

			if(g_tag_lrc == lrc(g_tag, 10))
			{
				if (g_mode == add && !is_master_tag(g_tag))
				{
					if(search_tag3() < 0)
					{
						tmp = get_empty_slot();
						
						if(tmp >= 0)
						{
							write_tag_into_flash(g_tag, tmp * 10);
							lcd_clear();
							lcd_put_str("Tag added:");
							lcd_next_line();
							put_tag(g_tag);
                            delay(10000);
                            lcd_clear();
							
						}
						else
						{
							lcd_clear();
							lcd_put_str("Err: mem full");
                            delay(10000);
                            lcd_clear();
						}
					}
					else
					{
						lcd_clear();
						lcd_put_str("Err: already in");
						lcd_next_line();
						put_tag(g_tag);
                        delay(10000);
                        lcd_clear();
					}
                    g_mode = read;
                }
				else if (g_mode == delete && !is_master_tag(g_tag))
				{
                    tmp = search_tag3();
					if(tmp > -1)
					{

                        delete_from_flash(tmp);
						lcd_clear();
						lcd_put_str("Tag deleted:");
						lcd_next_line();
						put_tag(g_tag);
                        delay(10000);
                        lcd_clear();
							
					}
					else
					{
						lcd_clear();
						lcd_put_str("Err: tag");
                        lcd_next_line();
                        lcd_put_str("not found");
                        delay(10000);
                        lcd_clear();
					}
                    g_mode = read;
				}
				else if (g_mode == read)
				{
					g_mode = get_tag_mode(g_tag);
					if (g_mode == read)
					{
						if(search_tag3() > -1)
						{
							lcd_clear();
							lcd_put_str("ACCESS");
							lcd_next_line();
							lcd_put_str("AUTHORIZED");
							READER_GREEN;
                            RELAY_OPEN;
							delay(50000);
							READER_ORANGE;
                            RELAY_CLOSE;
                            lcd_clear();
						}
						else
						{
							lcd_clear();
							lcd_put_str("ACCESS");
							lcd_next_line();
							lcd_put_str("UNAUTHORIZED");
							READER_RED;
							delay(10000);
							READER_ORANGE;
                            lcd_clear();

						}
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
                else
                {
                    lcd_clear();
					lcd_put_str("Err: op denied");
                    delay(10000);
                    lcd_clear();
                    g_mode = read;
                }
			}

			else
			{
				lcd_clear();
				lcd_put_str("Err: bad LRC");
			}


			g_nb_bit_read = 0;

		}

		WDTCONbits.WDTCLR = 1;	// Clear watchdog
	}
	return (0);
}

void __ISR(_EXTERNAL_1_VECTOR, IPL1AUTO) btn_int(void)
{
    /*
    u8 i = 0;
    if (i == 50)
        i = 0;
    u8 buffer[10] = {0};
    
    flash_read(buffer, 0, 10);
    
    lcd_clear();
	lcd_put_str("SR:");
    lcd_put_hex(flash_read_status_register());
    lcd_next_line();
	put_tag(buffer);
    i += 10;
    //*/
    
    //*
    flash_sector_erase(0x0);
    lcd_clear();
    lcd_put_str("Memorie erased");
    //*/
    
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
    g_timeout--;
    
    lcd_put_countdown(g_timeout);
    
    if (g_timeout == 0)
    {
        lcd_clear();
        T1CONbits.ON = 0; // Stop Timer 1
        g_mode = read;
        g_timeout = 9;
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