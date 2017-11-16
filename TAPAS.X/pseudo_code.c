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
//u8 g_page[400][10] = {{0}};

enum e_mode g_mode = read;

/*******************************************************************************
 * DESCRIPTION:
 *	The AAI program instruction allows multiple bytes
 *	of data to be programmed without re-issuing the
 *	next sequential address location.
 *
 * PARAMETERS:
 *	unsigned char *buf - buffer containing data to be send
 *	long addr - starting address
 *	int len - number of bytes to be written. No less than 2 bytes
 *
 * RETURN:
 *	none
 ******************************************************************************/


#include "spi.h"
#include "util.h"

void spi2_init()
{
		SPI2CON = 0;					// stop and reset SPI2CON
		SPI2BUF = 0;
		SPI2BRG = 0;					//set baudrate 2MHz
		SPI2CONbits.CKE = 1;			// CKE: SPI Clock Edge Select bit 1 = Serial output data changes on transition from active clock state to idle clock state (see CKP bit) 0 = Serial output data changes on transition from idle clock state to active clock state (see CKP bit)
		SPI2CONbits.SMP = 0;
		SPI2CONbits.CKP = 0;			// Clock Polarity Select bit 1 = Idle state for clock is a high level; active state is a low level 0 = Idle state for clock is a low level; active state is a high level
		SPI2CONbits.SSEN = 0;
		SPI2CONbits.MSTEN = 1;			// Master Mode Enable bit
		//SPI2CONbits.MODE32 = 1;		// mode 32bit
		SPI2CONbits.ON = 1;				// enable SPI2
		SPI2BRG = 50;
}
/*

u32 spi2_transmit32bits(u32 msg)
{
	//SPI2CONbits.MODE32 = 1;
	//FLASH_WP = HIGH;
	//FLASH_HOLD = HIGH;
	u32 res;

	//SPI2CONbits.MODE32 = 1;
	SPI2BUF = msg;
	FLASH_CE = LOW;

	while (!SPI2STATbits.SPIRBF) ;
	res = SPI2BUF;
	FLASH_CE = HIGH;
	return (res);
}

u32 spi2_transmit8bits(u8 msg)
{
	u32 res;

	FLASH_CE = LOW;
	SPI2BUF = msg;
	while (!SPI2STATbits.SPIRBF);
	FLASH_CE = HIGH;
	res = SPI2BUF;

	return (res);
}
*/

/*******************************************************************************
 * DESCRIPTION:
 *	Look if a given tag is empty (0xFF)
 *
 * PARAMETERS:
 *	u8 *buffer: Pointer to a tag.
 *
 * RETURN:
 *	1 if the tag is not empty, 0 if the tag is empty
 ******************************************************************************/

u8 is_not_empty(u8 *buffer)
{
    u8 i = 0;
    u8 flag = 0;

    while(i < 10 && !flag)
    {
        if (buffer[i++] != 0xff)
        {
            flag = 1;
        }
    }
    return (flag);
}

u8 is_empty(u8 *buffer)
{
    u8 i = 0;
    u8 flag = 0;

    while(i < 10 && !flag)
    {
        if (buffer[i++] != 0xff)
        {
            flag = 1;
        }
    }
    return (!flag);
}

u8 nb_empty_zone(u8 len)
{
    u32 addr = 0;
    u8 buffer[10] = {0};

    u8 count = 0;

    while(addr < 10 * len)
    {
        flash_read(buffer, addr, 10);

        if (!is_not_empty(buffer))
        {
            count ++;
        }
        addr += 10;
    }
    return (count);
}

s8 get_empty_slot(void)
{
    u32 addr = 0;
    u8 buffer[10] = {0};

    while(addr < 4000)
    {
        flash_read(buffer, addr, 10);

        if (!is_not_empty(buffer))
        {
            return (addr/10);
        }
        addr += 10;
    }
    return (-1);
}



void write_tag_into_flash(u8 *tag, u32 addr)
{
    u8 i = 0;

    while(i < 10)
    {
        flash_byte_program(tag[i], addr);
        addr++;
        i++;
    }

}

u8 get_empty_slot_index()
{
    u32 addr = 0;
    u8 i = 0;
    u8 buffer[10] = {0};

    u8 found = 0;

    while(addr < 4000 && !found)
    {
        flash_read(buffer, addr, 10);

        if (!is_not_empty(buffer))
        {
            found = 1;
        }
        addr += 10;
        i++;
    }
    return (i - 1);
}


u8  search_tag(u8 *tag, u32 *addr)
{
    *addr = 0x000000;
    u8 found = 0;

    u8 buffer[10] = {0};

    while(*addr < 4000 && !found)
    {
        flash_read(buffer, *addr, 10);

        if (!str_cmp_size(tag, buffer, 10))
        {
            found = 1;
        }
        else
        {
            *addr += 10;
        }
    }
    return(found);
}

s8  search_tag2(u8 *tag)
{
    u32 addr = 0x0;
    u8 found = 0;

    u8 buffer[10] = {0};

    while(addr <= 4000)
    {
        flash_read(buffer, addr, 10);

        if (!str_cmp_size(tag, buffer, 10))
        {
            return(addr/10);
        }
        else
        {
            addr += 10;
        }
    }
    return(-1);
}


void read_flash_in_buffer(void)
{
    u8 i = 0;

    while(i < 400)
    {
        flash_read(g_page[i], 0x00, 10);
        i++;
    }
}

void delete_tag_from_buffer_at_index(u8 index)
{
    u8 i = 0;

    while(i < 10)
    {
        g_page[index][i] = 0xff;
        i++;
    }
}





void delete_tag_from_buffer(u8 *tag)
{
    u8 i = 0;
    u8 found = 0;

    while(!found && i < 400)
    {
        if(str_cmp_size(tag, g_page[i], 10))
        {
            delete_tag_from_buffer_at_index(i);
            found = 1;
        }
        i++;
    }
}

u8 search_tag_in_buffer(u8 *tag)
{
    u8 found = 0;
    u8 i = 0;

    while(!found && i < 400)
    {
        if(str_cmp_size(tag, g_page[i], 10))
        {
            found = 1;
        }
        i++;
    }
}

void flash_read(u8 *buffer, u32 addr, u8 len)
{
	FLASH_CE = LOW;
	spi2_send_byte_continuous(FLASH_READ);
	spi2_send_byte_continuous((addr & 0xff0000) >> 16);
	spi2_send_byte_continuous((addr & 0x00ff00) >> 8);
	spi2_send_byte_continuous(addr & 0x0000ff);
	while(len-- > 0)
		*buffer++ = spi2_send_byte_continuous(0);
	FLASH_CE = HIGH;
}

void flash_write_enable(void)
{
    u8 junk;

	junk = spi2_send_byte(FLASH_WRITE_ENABLE);
}

void flash_clear_write_protection(void)
{
	u8 temp;
	temp = flash_read_status_register();
	temp &= 0xf3; // 11110011
	flash_write_enable();
	FLASH_CE = LOW;
	spi2_send_byte_continuous(FLASH_WRITE_STATUS_REG);
	spi2_send_byte_continuous(temp);
	FLASH_CE = HIGH;
	flash_write_disable();
}

void flash_sector_erase(u32 addr)
{
	flash_write_enable();
	FLASH_CE = LOW;
	spi2_send_byte_continuous(FLASH_4KB_SECOTR_ERASE);
	spi2_send_byte_continuous((addr & 0xff0000) >> 16);
	spi2_send_byte_continuous((addr & 0x00ff00) >> 8);
	spi2_send_byte_continuous(0);
	FLASH_CE = HIGH;
	//pool busy flag
	while((flash_read_status_register() & 0x01) == 1);
	flash_write_disable();
}

void flash_write_disable(void)
{
    u8 junk;

	junk = spi2_send_byte(FLASH_WRITE_DISABLE);
}

void flash_byte_program(u8 data, u32 addr)
{
    u8 junk;

	flash_write_enable();
	FLASH_CE = LOW;
	junk = spi2_send_byte_continuous(FLASH_BYTE_PROGRAM);
	junk = spi2_send_byte_continuous((addr & 0xff0000) >> 16);
	junk = spi2_send_byte_continuous((addr & 0x00ff00) >> 8);
	junk = spi2_send_byte_continuous(addr & 0x0000ff);
	junk = spi2_send_byte_continuous(data);	//DIN
	FLASH_CE = HIGH;
	flash_write_disable();
}

/*
void spi2_read_rdsr(void)
{
	u8 res;
	u8 junk;

	FLASH_CE = LOW;

	SPI2BUF = 0x5;
	while (!SPI2STATbits.SPIRBF);
	res = SPI2BUF;

	SPI2BUF = junk;
	while (!SPI2STATbits.SPIRBF);
	res = SPI2BUF;

	FLASH_CE = HIGH;

}
*/

u8 spi2_send_byte(u8 data)
{
	u8 res;

    FLASH_CE = LOW;
	SPI2BUF = data;
	while(!SPI2STATbits.SPIRBF);
	res = SPI2BUF;
    FLASH_CE = HIGH;
	return (res);
}

u8 spi2_send_byte_continuous(u8 data)
{
	u8 res;

	SPI2BUF = data;
	while(!SPI2STATbits.SPIRBF);
	res = SPI2BUF;
	return (res);
}

void spi2_read_Jedec(u8 *buf)
{
    u8 junk;

	FLASH_CE = LOW;
	junk = spi2_send_byte_continuous(FLASH_READ_JEDEC_ID);
	*buf++ = spi2_send_byte_continuous(0);
	*buf++ = spi2_send_byte_continuous(0);
	*buf = spi2_send_byte_continuous(0);
	FLASH_CE = HIGH;
}

u8 flash_read_status_register(void)
{
	u8 junk;
    u8 res;

	FLASH_CE = LOW;
	junk = spi2_send_byte_continuous(FLASH_READ_STATUS_REG);
	res = spi2_send_byte_continuous(0);
	FLASH_CE = HIGH;
	return res;
}

/*
void spi2_read_jedec2(void)
{
	u8 res[3] = {0};
	u8 junk;

	FLASH_CE = LOW;

	SPI2BUF = 0x9f;
	while (!SPI2STATbits.SPIRBF);
	res[0] = SPI2BUF;

	SPI2BUF = junk;
	while (!SPI2STATbits.SPIRBF);
	res[0] = SPI2BUF;


	SPI2BUF = junk;
	while (!SPI2STATbits.SPIRBF);
	res[1] = SPI2BUF;


	SPI2BUF = junk;
	while (!SPI2STATbits.SPIRBF);
	res[2] = SPI2BUF;

	FLASH_CE = HIGH;
}
*/

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

	flash_clear_write_protection();

	while (1)
	{
		if(g_nb_bit_read == 44)
		{
			disable_timout();

			if(g_tag_lrc == lrc(g_tag, 10))
			{

				if(!search_tag2(g_tag))
				{
					if(nb_empty_zone(1) > 0)
					{
						lcd_clear();
						lcd_put_str("Write");
						flash_byte_program(5, 0x0);


					}
					else
					{
						lcd_clear();
						lcd_put_str("Mem full");
						flash_sector_erase(0x0);

					}

				}
				else
				{
					lcd_clear();
					lcd_put_str("Already in");
				}



			}

			else
			{
				lcd_clear();
				lcd_put_str("LRC Error");
			}

			g_nb_bit_read = 0;

		}

		WDTCONbits.WDTCLR = 1;	// Clear watchdog
	}
	return (0);
}

void __ISR(_EXTERNAL_1_VECTOR, IPL1AUTO) btn_int(void)
{
	u8 buffer[10] = {0};

	flash_read(buffer, 0x0, 10);

	lcd_clear();
	lcd_put_str("SR:");
	lcd_put_hex(flash_read_status_register());
	lcd_next_line();
	put_tag(buffer);

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
