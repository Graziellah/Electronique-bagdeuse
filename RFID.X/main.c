#include <sys/attribs.h>
//#include <plib.h> // Microchip PIC32MX Peripheral Library

//Defines

#include <stdint.h>
#include <proc/p32mx340f512h.h>

#define SYSCLK (1000000UL)
#define BAUDRATE (9600)
#define INPUT 1
#define OUTPUT 0
#define HIGH 1
#define LOW 0

// LECTEUR RFID
#define DATA_0 LATDbits.LATD9
#define DATA_1 LATDbits.LATD10

// Typedefs
typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long int u32;

typedef char s8;
typedef int s16;
typedef long int s32;

// Globales
u8 g_tag[10] = {0};
u8 g_flag = 0;
u8 g_tag_lrc = 0;
u8 g_nb_bit_read = 0;
u8 g_data_index = 0;

void init_GPIO()
{
    // Set Button pin as INPUT
    TRISDbits.TRISD8 = INPUT;

    // Set DATA_0 & DATA_1 pin as INPUTS
    TRISDbits.TRISD9 = INPUT;
    TRISDbits.TRISD10 = INPUT;
}

u8    lrc(u8* data, u8 size)
{
    u8  lrc = 0;
    while(size)
    {
        lrc ^= *data++;
        size--;
    }
    return(lrc);
}

void    init_int()
{
    //ISR button
    IFS0bits.INT1IF = 0; //Unset the flag
    IEC0bits.INT1IE = 1; //Enable the button interrupt
    IPC1bits.INT1IP = 1; //Set the priority to 1

    //ISR DATA_0
    IFS0bits.INT2IF = 0; //Unset the flag
    IEC0bits.INT2IE = 1; //Enable the button interrupt
    IPC2bits.INT2IP = 2; //Set the priority to 7

    //ISR DATA_1
    IFS0bits.INT3IF = 0; //Unset the flag
    IEC0bits.INT3IE = 1; //Enable the button interrupt
    IPC3bits.INT3IP = 2; //Set the priority to 7

    INTCONbits.MVEC = 1; // Enable multi vector
    __builtin_enable_interrupts(); // Enable interruption
}

void    init_UART2(void)
{
    U2STAbits.URXEN = 0; // Reciver Enable bit
    U2STAbits.UTXEN = 1; // Transmit Enable bit
    U2BRG = ((SYSCLK)/16/(BAUDRATE)-1);
    U2MODEbits.ON = 1;   // UART Enable bit
}

void    UART2_put_char(const char c)
{
    while (U2STAbits.UTXBF);
    U2TXREG = c;
}

void    UART2_put_str(const char* str)
{
    u8 i = 0;

    while(str[i])
    {
        while (U2STAbits.UTXBF);
        UART2_put_char(str[i++]);
    }
}

void    UART2_put_raw_data_32(u32 data)
{
    u8 i = 0;
    while(U2STAbits.UTXBF);
    while(i <= 32)
    {
        UART2_put_char(((data >> i++) & 0b1) + 48);
    }
}

void    UART2_put_4bit_size(u8* data, u8 size)
{
    u8 i = 0;
    u8 j = 0;
    while(U2STAbits.UTXBF);
    while(i++ <= size)
    {
        j = 0;
        while(j <= 4)
        {
            UART2_put_char(((data[i] >> j++) & 0b1) + 48);
        }
    }
}

void    add_bit(u8 b)
{
    if (g_nb_bit_read == 0)
    {
        // vider le tableau
    }

    g_nb_bit_read++;
    if (g_nb_bit_read < 40)
    {
        if (g_nb_bit_read % 4 == 0)
        {
            g_data_index++;
        }
        if (b)
        {
            g_tag[g_data_index] <<= 1;
            g_tag[g_data_index]+= 1;
        }
        else
        {
            g_tag[g_data_index] <<= 1;
        }
    }
    else if (g_nb_bit_read > 40 && g_nb_bit_read < 44)
    {

        if (g_nb_bit_read %4 == 1)
        {
            g_tag_lrc = 0;
        }

        if (b)
        {
            g_tag_lrc <<= 1;
            g_tag_lrc += 1;
        }
        else
        {
            g_tag_lrc <<= 1;
        }

    }
    else if (g_nb_bit_read == 44)
    {
        g_data_index = 0;
        g_nb_bit_read = 0;
        g_flag = 1;
    }
}

int main()
{
    //SYSTEMConfigPerforman.ce(SYSCLK);

    init_GPIO();
    init_int();
    init_UART2();

    while (1)
    {
        if(g_flag)
        {
            g_flag = 0;
            if(g_tag_lrc = lrc(g_tag, 10))
            {
                UART2_put_str("Tag lu: \r\n");
                UART2_put_4bit_size(g_tag, 10);

                // si tag maitre ajout > mode d'ajout (avec auto sleep pour ne pas declanger watchdog)
                // sinon si tag maitre sup > mode supp (pareil que pour l'ajout)
                // sinon verifie si le tag est authorise (le trouver dans la flash)
                // si oui, declangher relai et lecteur led verte
                // si introuvable dans la flash // lecteur led rouge
            }
            else
            {
                UART2_put_str("LRC Error\r\n");
            }
        }
	WDTCONbits.WDTCLR = 1; //Pet the dog
    }
    return (0);
}

void __ISR(_EXTERNAL_1_VECTOR, IPL1AUTO) int_btn(void)
{
    UART2_put_str("UART2:\r\n");
    IFS0bits.INT1IF = 0;
}

void __ISR(_EXTERNAL_2_VECTOR, IPL2AUTO) int_data_0(void)
{
    add_bit(0);
    IFS0bits.INT2IF = 0;
}

void __ISR(_EXTERNAL_3_VECTOR, IPL2AUTO) int_data_1(void)
{
    add_bit(1);
    IFS0bits.INT3IF = 0;
}
