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

#define FLASH_CE LATGbits.LATG9 // Flash CE pin, SPI2CON SSEN = 0 donc le pin RG9 est controler par les port fonctions
#define FLASH_WP LATDbits.LATD0 // Write Protection is active low
#define FLASH_HOLD LATDbits.LATD1 // Hold is active low

// Typedefs
typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long int u32;
typedef char s8;
typedef int s16;
typedef long int s32;


u8	strlen(u8 *str)
{
  u8 i = 0;
  while (str[i] != '\0')
    i++;
  return (i);
}



void init_IO()
{

    LED_OFF;
    TRISFbits.TRISF1 = OUTPUT;
    FLASH_CE = HIGH;
    TRISGbits.TRISG9 = OUTPUT; // FLASH CE pin
    FLASH_WP = HIGH;
    TRISDbits.TRISD0 = OUTPUT; // Flash WP pin
    FLASH_HOLD = HIGH;
    TRISDbits.TRISD1 = OUTPUT; // Flash HOLD pin

    TRISDbits.TRISD8 = INPUT; //Button
}


void    init_int()
{
    // Clear SPI2 Interrupt
    IEC1bits.SPI2EIE = 0;
    IEC1bits.SPI2RXIE = 0;
    IEC1bits.SPI2TXIE = 0;

    // Clear SPI2 Interrupt flag
    IFS1bits.SPI2EIF = 0;
    IFS1bits.SPI2RXIF = 0;
    IFS1bits.SPI2TXIF = 0;

    //ISR button
    IFS0bits.INT1IF = 0; //Unset the flag
    IEC0bits.INT1IE = 1; //Enable the button interrupt
    IPC1bits.INT1IP = 1; //Set the priority

    INTCONbits.MVEC = 1;
    __builtin_enable_interrupts();
}

void    init_SPI2()
{
        SPI2CON = 0; // stop and reset SPI2CON
        SPI2BUF = 0;
        SPI2BRG = 0; //set baudrate 2MHz
        SPI2CONbits.CKE = 1; // CKE: SPI Clock Edge Select bit 1 = Serial output data changes on transition from active clock state to idle clock state (see CKP bit) 0 = Serial output data changes on transition from idle clock state to active clock state (see CKP bit)
        SPI2CONbits.SMP = 0;
        SPI2CONbits.CKP = 0; // Clock Polarity Select bit 1 = Idle state for clock is a high level; active state is a low level 0 = Idle state for clock is a low level; active state is a high level
        SPI2CONbits.SSEN = 0;
        SPI2CONbits.MSTEN = 1; // Master Mode Enable bit
        //SPI2CONbits.MODE32 = 1; // mode 32bit
        SPI2CONbits.ON = 1; // enable SPI2
        SPI2BRG = 50;
}
void    init_UART2()
{
    U2STAbits.URXEN = 0; // Reciver Enable bit
    U2STAbits.UTXEN = 1; // Transmit Enable bit
    U2BRG = ((SYSCLK)/16/(BAUDRATE)-1);
    U2MODEbits.ON = 1;   // UART Enable bit
}

u32 SPI2_transmit32bits(u32 msg)
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

u32 SPI2_transmit8bits(u8 msg)
{
    u32 res;

    FLASH_CE = LOW;
    SPI2BUF = msg;
    while (!SPI2STATbits.SPIRBF);
    FLASH_CE = HIGH;
    res = SPI2BUF;

    return (res);
}

void      SPI2_read_RDSR()
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

void      SPI2_read_JEDEC()
{
    u8 res;
    
    u8 junk;

    FLASH_CE = LOW;
    SPI2BUF = 0x9f;
    while (!SPI2STATbits.SPIRBF);
    res = SPI2BUF;

    SPI2BUF = junk;
    while (!SPI2STATbits.SPIRBF);
    res = SPI2BUF;

    SPI2BUF = junk;
    while (!SPI2STATbits.SPIRBF);
    res = SPI2BUF;

    SPI2BUF = junk;
    while (!SPI2STATbits.SPIRBF);
    res = SPI2BUF;


    FLASH_CE = HIGH;

    //res = SPI2_transmit32bits(0x9f);
    //res = SPI2_transmit32bits(0x9f);
    //res = SPI2_transmit8bits(0x9f);
    //res = SPI2_transmit32bits(0b11111001);

    //res = SPI2_transmit32bits(0b11111001);
    //1001 1111
    //SPI2BUF = (0x9F);
    //SPI2BUF = (0b10100000111100001111101010011001);
}

u32      SPI2_readID()
{
    u8 res;
    //res = SPI2_transmit32bits(0x90);
    res = SPI2_transmit8bits(0x90);
    return (res);
}
/*
void    readFlashFromAddr(u32 addr)
{
    u8 read = 0b00000011;
    SPI2BUF = (addr << 8) | read;
    CE_ENABLE;
}
*/

void    writeCharUART2(const char c)
{
    while (U2STAbits.UTXBF); //or while(!U1STAbits.TRMT);
    U2TXREG = c;
}

void    writeStringUART2(const char* str)
{
    u8 i = 0;

    while(str[i])
    {
        while (U2STAbits.UTXBF); //or while(!U1STAbits.TRMT);
        U2TXREG = (str[i++]);
    }
}

void    writeDataUART2(u8 data)
{
    u8 i = 0;
    while(U2STAbits.UTXBF);
    while(i <= 8)
    {
        writeCharUART2(((data >> i++) & 0b1) + 48);
    }
}

int main()
{
     int pbClk;
    pbClk = SYSTEMConfigPerformance(SYSCLK);

    init_UART2();
    init_SPI2();
    init_int();
    init_IO();

    while (1)
    {
        WDTCONbits.WDTCLR = 1; //Pet the dog
    }
    return (0);
}

void __ISR(_EXTERNAL_1_VECTOR, IPL1AUTO) ButtonHandler(void)
{
    SPI2_read_JEDEC();
    //SPI2_read_RDSR();
    IFS0bits.INT1IF = 0;
}

