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

u8 is_not_empty(u8 *buffer)
{
    u8 i = 0;
    u8 flag = 0;
    
    while(i < 10 && !flag)
    {
        if (buffer[i++] != 0)
        {
            flag = 1;
        }
    }
    return (flag);
}

u8 nb_empty_zone()
{
    u32 addr = 0;
    u8 buffer[10] = {0};
    
    u8 count = 0;
    
    while(addr <= 0x000064)
    {
        flash_read(buffer, addr, 10);

        if (!is_not_empty(buffer))
        {
            count ++;
        }
    }
    return (count);
}

u8  search_tag(u8 *tag, u32 *addr)
{
    *addr = 0x000000;
    u8 found = 0;
    
    u8 buffer[10] = {0};
 
    while(*addr <= 0x000064 && !found)
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

u8 spi2_send_byte(u8 data)
{
	u8 res;
    
    FLASH_CE = LOW;
	SPI2BUF = data;
	while(!SPI2STATbits.SPIRBF);
	res = SPI2BUF;
    FLASH_CE = HIGH;
	return res;
}

u8 spi2_send_byte_continuous(u8 data)
{
	u8 res;
    
	SPI2BUF = data;
	while(!SPI2STATbits.SPIRBF);
	res = SPI2BUF;
	return res;
}

void spi2_read_Jedec(u8 *buf)
{
	FLASH_CE = LOW;
	spi2_send_byte_continuous(FLASH_READ_JEDEC_ID);
	*buf++ = spi2_send_byte_continuous(0);
	*buf++ = spi2_send_byte_continuous(0);
	*buf = spi2_send_byte_continuous(0);
	FLASH_CE = HIGH;
}

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
