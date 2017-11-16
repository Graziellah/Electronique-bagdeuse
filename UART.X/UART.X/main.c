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

// Typedefs
typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long int u32;
typedef char s8;
typedef int s16;
typedef long int s32;

void    initInt()
{
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

void    writeStringUART2(const char* str)
{
    u8 i = 0;
    
    while(str[i])
    {
        while (U2STAbits.UTXBF); //or while(!U1STAbits.TRMT);  
        U2TXREG = (str[i++]);
    }
}

int main()
{   
    int pbClk;
    pbClk = SYSTEMConfigPerformance(SYSCLK);
    
    initUART2(pbClk);
    initInt();
	while (1)
	{
		WDTCONbits.WDTCLR = 1; //Pet the dog
	}
	return (0);
}

void __ISR(_EXTERNAL_1_VECTOR, IPL1AUTO) ButtonHandler(void)
{
	writeStringUART2("TEST123\r\n");
    
	IFS0bits.INT1IF = 0;
}
            
