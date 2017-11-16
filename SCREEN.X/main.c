#include <sys/attribs.h>
#include <plib.h> // Microchip PIC32MX Peripheral Library

//Defines

#include <stdint.h>
#include <sys/attribs.h>
#include <proc/p32mx340f512h.h>

#define SYSCLK (8000000)
#define BAUDRATE (9600)
#define INPUT 1
#define OUTPUT 0
#define HIGH 1
#define LOW 0
#define INVERT_LED (LATFINV = 0b10)
#define LED_OFF (LATFbits.LATF1 = 0)
#define LED_ON (LATFbits.LATF1 = 1)
#define BUTTON_PRESSED (PORTD)

//Flash
#define FLASH_CE_ON (LATGbits.LATG9 = LOW); // Flash CE pin, SPI2CON SSEN = 0 donc le pin RG9 est controler par les port fonctions
#define FLASH_CE_OFF (LATGbits.LATG9 = HIGH);
#define FLASH_WP_ON (LATDbits.LATD0 = LOW) // write protection is active low
#define FLASH_WP_OFF (LATDbits.LATD0 = HIGH)
#define FLASH_HOLD_OFF (LATDbits.LATD1 = HIGH) // Hold is active low
#define FLASH_HOLD_ON (LATDbits.LATD1 = LOW)

//LCD
#define LCD_DB0 LATEbits.LATE0
#define LCD_DB1 LATEbits.LATE1
#define LCD_DB2 LATEbits.LATE2
#define LCD_DB3 LATEbits.LATE3
#define LCD_DB4 LATEbits.LATE4
#define LCD_DB5 LATEbits.LATE5
#define LCD_DB6 LATEbits.LATE6
#define LCD_DB7 LATEbits.LATE7
#define LCD_ENABLE LATDbits.LATD0
#define LCD_RS LATDbits.LATD1
#define LCD_RW LATDbits.LATD2

// Typedefs
typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long int u32;
typedef char s8;
typedef int s16;
typedef long int s32;

void initIO()
{
    //LCD
    LCD_DB0 = LOW;
    LCD_DB1 = LOW;
    LCD_DB2 = LOW;
    LCD_DB3 = LOW;
    LCD_DB4 = LOW;
    LCD_DB5 = LOW;
    LCD_DB6 = LOW;
    LCD_DB7 = LOW;

    TRISEbits.TRISE0 = OUTPUT;
    TRISEbits.TRISE1 = OUTPUT;
    TRISEbits.TRISE2 = OUTPUT;
    TRISEbits.TRISE3 = OUTPUT;
    TRISEbits.TRISE4 = OUTPUT;
    TRISEbits.TRISE5 = OUTPUT;
    TRISEbits.TRISE6 = OUTPUT;
    TRISEbits.TRISE7 = OUTPUT;
    TRISDbits.TRISD1 = OUTPUT;
    TRISDbits.TRISD2 = OUTPUT;

    LCD_ENABLE = LOW;
    TRISDbits.TRISD0 = OUTPUT;

    LCD_RS = LOW;
    TRISDbits.TRISD1 = OUTPUT;

    LCD_RW = LOW;
    TRISDbits.TRISD2 = OUTPUT;
}

void    initInt()
{
    // Clear SPI2 Interrupt
    IEC1bits.SPI2EIE = 0;
    IEC1bits.SPI2RXIE = 0;
    IEC1bits.SPI2TXIE = 0;

    // Clear SPI2 Interrupt flag
    IFS1bits.SPI2EIF = 0;
    IFS1bits.SPI2RXIF = 0;
    IFS1bits.SPI2TXIF = 0;

    TRISDbits.TRISD8 = INPUT; //Button
    //ISR button
    IFS0bits.INT1IF = 0; //Unset the flag
    IEC0bits.INT1IE = 1; //Enable the button interrupt
    IPC1bits.INT1IP = 1; //Set the priority to 7

    INTCONbits.MVEC = 1;
    __builtin_enable_interrupts();
}

void    initUART2(int pbClk)
{
    U2MODEbits.ON = 1;   // UART Enable bit
    U2STAbits.URXEN = 0; // Reciver Enable bit
    U2STAbits.UTXEN = 1; // Transmit Enable bit
    U2BRG = ((pbClk)/16/(BAUDRATE)-1);
}

void    lcdRWMode(s8 enable)
{
    LCD_DB0 = LOW;
    LCD_DB1 = LOW;
    LCD_DB2 = LOW;
    LCD_DB3 = LOW;
    LCD_DB4 = LOW;
    LCD_DB5 = LOW;
    LCD_DB6 = LOW;
    LCD_DB7 = LOW;
    if (enable)
    {
        TRISEbits.TRISE0 = INPUT;
        TRISEbits.TRISE1 = INPUT;
        TRISEbits.TRISE2 = INPUT;
        TRISEbits.TRISE3 = INPUT;
        TRISEbits.TRISE4 = INPUT;
        TRISEbits.TRISE5 = INPUT;
        TRISEbits.TRISE6 = INPUT;
        TRISEbits.TRISE7 = INPUT;
        TRISDbits.TRISD1 = INPUT;
        TRISDbits.TRISD2 = INPUT;
    }
    else
    {
        TRISEbits.TRISE0 = OUTPUT;
        TRISEbits.TRISE1 = OUTPUT;
        TRISEbits.TRISE2 = OUTPUT;
        TRISEbits.TRISE3 = OUTPUT;
        TRISEbits.TRISE4 = OUTPUT;
        TRISEbits.TRISE5 = OUTPUT;
        TRISEbits.TRISE6 = OUTPUT;
        TRISEbits.TRISE7 = OUTPUT;
        TRISDbits.TRISD1 = OUTPUT;
        TRISDbits.TRISD2 = OUTPUT;
    }
}

void    clearLCD()
{
    LCD_RS = LOW;
    LCD_RW = LOW;

    LCD_DB0 = HIGH;
    LCD_DB1 = LOW;
    LCD_DB2 = LOW;
    LCD_DB3 = LOW;
    LCD_DB4 = LOW;
    LCD_DB5 = LOW;
    LCD_DB6 = LOW;
    LCD_DB7 = LOW;

    LCD_ENABLE = HIGH;
    while(lcdBusy());
    LCD_ENABLE = LOW;
}

u8      lcdBusy()
{
    u8 busy;

    lcdRWMode(1);
    LCD_RS = LOW;
    LCD_RW = HIGH;
    u8 busy = LCD_DB7;
    lcdRWMode(0);
    return (busy);
}

void    putcLCD(char c)
{
    LCD_DB0 = (c >> 0) & 0b1;
    LCD_DB1 = (c >> 1) & 0b1;
    LCD_DB2 = (c >> 2) & 0b1;
    LCD_DB3 = (c >> 3) & 0b1;
    LCD_DB4 = (c >> 4) & 0b1;
    LCD_DB5 = (c >> 5) & 0b1;
    LCD_DB6 = (c >> 6) & 0b1;
    LCD_DB7 = (c >> 7) & 0b1;

    while(lcdBusy());
    LCD_ENABLE = HIGH;
    while(lcdBusy());
    LCD_ENABLE = LOW;
}

void    putsLCD(char *str)
{
    u8 i = 0;
    while(str[i])
    {
        putcLCD(str[i++]);
    }
}

char    writeSPI(char data)
{
    SPI2BUF = data; // TX Data
    // wait until spi ready
    //SPI2STATbits.
    data = SPI2BUF;
    return (data);
}

u32      flashReadJEDEC()
{
    u8 junk;
    junk = SPI2BUF;
    FLASH_CE_OFF;
    SPI2BUF = (0x9F);
    while(SPI2STATbits.SPIBUSY);// wait until SPI is in idle mode
    FLASH_CE_ON;
    while(!SPI2STATbits.SPITBE); // wait until transmit buffer empty

    while(SPI2STATbits.SPIBUSY);// wait until SPI is in idle mode

    FLASH_CE_OFF;
   return (SPI2BUF);
}
/*
void    readFlashFromAddr(u32 addr)
{
    u8 read = 0b00000011;
    SPI2BUF = (addr << 8) | read;
    CE_ENABLE;
}
*/

void    writeStringUART2(const char* str)
{
    u8 i = 0;

    while(str[i])
    {
        while (U2STAbits.UTXBF); //or while(!U1STAbits.TRMT);
        U2TXREG = (str[i++]);
    }
}

void    writeDataUART2(u32 data)
{
    u8 i = 0;
    while (U2STAbits.UTXBF);
    U2TXREG = (data);
}

int main()
{
    int pbClk;
    pbClk = SYSTEMConfigPerformance(SYSCLK);

    initUART2(pbClk);
    initSPI2();
    initInt();
    initIO();

	while (1)
	{
		WDTCONbits.WDTCLR = 1; //Pet the dog
	}
	return (0);
}

void __ISR(_EXTERNAL_1_VECTOR, IPL1AUTO) ButtonHandler(void)
{
        u32 data = flashReadJEDEC();
        writeStringUART2("TEST\r\n");
        writeDataUART2(data);
        writeStringUART2("\r\n");
	IFS0bits.INT1IF = 0;
}

