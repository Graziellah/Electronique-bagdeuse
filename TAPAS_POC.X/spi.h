#include "main.h"

#ifndef SPI_H
#define SPI_H

#define FLASH_READ                      0x03
#define FLASH_4KB_SECOTR_ERASE          0x20
#define FLASH_HI_SPEED_READ             0x0b
#define FLASH_32KB_BLOCK_ERASE          0x52
#define FLASH_64KB_BLOCK_ERASE          0xd8
#define FLASH_CHIP_ERASE				0x60
#define FLASH_BYTE_PROGRAM              0x02
#define FLASH_AAI_WORD_PROGRAM          0xad
#define FLASH_READ_STATUS_REG			0x05
#define FLASH_ENABLE_WRITE_STATUS_REG	0x50
#define FLASH_WRITE_STATUS_REG          0x01
#define FLASH_WRITE_ENABLE              0x06
#define FLASH_WRITE_DISABLE             0x04
#define FLASH_READ_ID					0x90
#define FLASH_READ_JEDEC_ID             0x9f
#define FLASH_ENABLE_SO_BUSY			0x70
#define FLASH_DISABLE_SO_BUSY			0x80

#define FLASH_CE LATGbits.LATG9		// Flash CE pin, SPI2CON SSEN = 0 donc le pin RG9 est controler par les port fonctions
#define FLASH_WP LATDbits.LATD0		// Write Protection is active low
#define FLASH_HOLD LATDbits.LATD1	// Hold is active low

void spi2_init();
u8 spi2_send_byte(u8 data);
u8 spi2_send_byte_continuous(u8 data);
void flash_read(u8 *buffer, u32 addr, u8 len);
void flash_byte_program(u8 data, u32 addr);
u8 flash_read_status_register(void);

void flash_write_enable(void);

void flash_write_disable(void);


#endif

