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

        if (is_empty(buffer))
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

void write_4Kflash_from_page_buffer(void)
{
    
}

void read_4Kflash_into_page_buffer(void)
{
    u8 i;
    u32 addr = 0;
    i = 0;
    
    while(i < 100)
    {
        flash_read(g_page[i], addr, 10);
        i++;
        addr += 10;
    }
}

s8  search_tag2()
{
    u32 addr = 0;
    u8 found = 0;
    
    u8 tmp_buffer[10] = {0};
    
    
 
    while(addr <= 4000)
    {
        flash_read(tmp_buffer, addr, 10);

        if (!str_cmp_size(g_tag, tmp_buffer, 10))
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

s16  search_tag3()
{
    u32 addr = 0;
    s16 found = -1;
    u16 i = 0;
     
    //read_4Kflash_into_page_buffer();
    //flash_read_4K();
    flash_high_speed_read_4K();
   
    while(i < 400 && found == -1)
    {
        if (!tag_cmp(g_tag, g_page[i]))
        {
            found = i;
        }
        i++;
 
    }
    return(found);
}

void delete_from_flash(u16 tmp)
{
    u16 i;
    
    flash_high_speed_read_4K();
    for (i = 0 ; i < 10 ; i++)
    {
        g_page[tmp][i] = 0xff;
    }
    flash_sector_erase(0x0);
    
    i = 0;
    
    while (i < 4000)
    {
        flash_byte_program(g_page[i / 10][i % 10], i);
        i++;
    }
    
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

void flash_high_speed_read_4K()
{
    s16 len = 0;
  
	FLASH_CE = LOW;
    spi2_send_byte_continuous(FLASH_HI_SPEED_READ);
	spi2_send_byte_continuous(0);
	spi2_send_byte_continuous(0);
	spi2_send_byte_continuous(0);
    spi2_send_byte_continuous(0);	//dummy bit
    while(len < 4000)
    {
        g_page[len / 10][len % 10] = spi2_send_byte_continuous(0);
        len++;
    }
    FLASH_CE = HIGH;
}


void flash_read_4K()
{    
    s16 len = 0;
   
	FLASH_CE = LOW;
	spi2_send_byte_continuous(FLASH_READ);
	spi2_send_byte_continuous(0);
	spi2_send_byte_continuous(0);
	spi2_send_byte_continuous(0);
	while(len < 4000)
    {
        g_page[len / 10][len % 10] = spi2_send_byte_continuous(0);
        len++;
    }
	FLASH_CE = HIGH;
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

void flash_high_speed_read(u8 *buffer, u32 addr, u8 len)
{
	FLASH_CE = LOW;
    spi2_send_byte_continuous(FLASH_HI_SPEED_READ);
    spi2_send_byte_continuous((addr & 0xff0000) >> 16);
    spi2_send_byte_continuous((addr & 0x00ff00) >> 8);
    spi2_send_byte_continuous(addr & 0x0000ff);
    spi2_send_byte_continuous(0);	//dummy bit
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