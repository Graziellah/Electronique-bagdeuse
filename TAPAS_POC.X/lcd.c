#include "lcd.h"

// Write to the display buffer the charactere code at the desired line (0 = line 1; 1 = line 2) and index
void lcd_display_write_buffer_c(u8 c, u8 l, u8 i)
{
	g_display_buffer[i + 16 * l] = c;
}

void lcd_display_write_buffer_str(u8 *str, u8 l, u8 i)
{
	while(*str)
	{
		lcd_display_write_buffer_c(*str, l, i);	// Put char
		i++;                                    // Shift cursor
		str++;                                  // Shift char addr
	}
}

void lcd_display_update()
{
	lcd_home();

	u8 i = 0;
	while (i < BUFFER_SIZE)
	{
		if (i == 16)
		{
			lcd_set_ddram_addr(0xc0); // Change line
		}
		lcd_write(g_display_buffer[i++], 1);
	}
}

void lcd_rw_mode(u8 enable)
{
	LCD_RS = LOW;
	LCD_RW = LOW;

	if (enable)
	{
		LCD_RW = HIGH;

		TRISEbits.TRISE7 = INPUT; // DB0
		TRISEbits.TRISE6 = INPUT; // DB1
		TRISEbits.TRISE5 = INPUT; // DB2
		TRISEbits.TRISE4 = INPUT; // DB3
		TRISEbits.TRISE3 = INPUT; // DB4
		TRISEbits.TRISE2 = INPUT; // DB5
		TRISEbits.TRISE1 = INPUT; // DB6
		TRISEbits.TRISE0 = INPUT; // DB7
	}
	else
	{
		LCD_DB0 = LOW;
		LCD_DB1 = LOW;
		LCD_DB2 = LOW;
		LCD_DB3 = LOW;
		LCD_DB4 = LOW;
		LCD_DB5 = LOW;
		LCD_DB6 = LOW;
		LCD_DB7 = LOW;

		TRISEbits.TRISE7 = OUTPUT; // DB0
		TRISEbits.TRISE6 = OUTPUT; // DB1
		TRISEbits.TRISE5 = OUTPUT; // DB2
		TRISEbits.TRISE4 = OUTPUT; // DB3
		TRISEbits.TRISE3 = OUTPUT; // DB4
		TRISEbits.TRISE2 = OUTPUT; // DB5
		TRISEbits.TRISE1 = OUTPUT; // DB6
		TRISEbits.TRISE0 = OUTPUT; // DB7
	}
}

/*
 * tl;dr Initialise the screen
 *
 * To-do: initialise the screen in 4bit mode
 */

void lcd_init()
{
	delay(100);

	LCD_RS = LOW;
	LCD_RW = LOW;
	LCD_ENABLE = LOW;

	TRISFbits.TRISF0 = OUTPUT;       // RS (Register selcct)
	TRISFbits.TRISF2 = OUTPUT;       // R/W (Read/Write)
	TRISFbits.TRISF3 = OUTPUT;       // E (Enable)

	LCD_DB0 = LOW;
	LCD_DB1 = LOW;
	LCD_DB2 = LOW;
	LCD_DB3 = LOW;
	LCD_DB4 = LOW;
	LCD_DB5 = LOW;
	LCD_DB6 = LOW;
	LCD_DB7 = LOW;

	TRISEbits.TRISE7 = OUTPUT;      // DB0
	TRISEbits.TRISE6 = OUTPUT;      // DB1
	TRISEbits.TRISE5 = OUTPUT;      // DB2
	TRISEbits.TRISE4 = OUTPUT;      // DB3
	TRISEbits.TRISE3 = OUTPUT;      // DB4
	TRISEbits.TRISE2 = OUTPUT;      // DB5
	TRISEbits.TRISE1 = OUTPUT;      // DB6
	TRISEbits.TRISE0 = OUTPUT;      // DB7

	lcd_function_set(1, 1, 0);      // 8 bits interface, 2 lines, 5x8 character font
	lcd_display_control(1, 0, 0);   // Display ON, cursor OFF, cursor blink OFF
	lcd_entry_mode_set(1, 0);
	lcd_clear();
	lcd_home();
}

u8 lcd_read_data()
{
	u8 res = 0;
	lcd_rw_mode(1);
	LCD_ENABLE = HIGH;
	res = 0x80 * LCD_DB7 + 0x40 * LCD_DB6 + 0x20 * LCD_DB5 + 0x10 * LCD_DB4 + 0x8 * LCD_DB3 + 0x4 * LCD_DB2 + 0x2 * LCD_DB1 + 0x1 * LCD_DB0;
	LCD_ENABLE = LOW;
	lcd_rw_mode(0);
	return(res);
}

u8 lcd_is_busy()
{
	u8 res = lcd_read_data();

	if (res & 0b10000000)
	{
		return (1);
	}
	return(0);
}

//Send data to the lcd
void	lcd_write(u8 data, u8 rs)
{
	LCD_RS = rs;

	LCD_DB0 = (data >> 0) & 0b1;
	LCD_DB1 = (data >> 1) & 0b1;
	LCD_DB2 = (data >> 2) & 0b1;
	LCD_DB3 = (data >> 3) & 0b1;
	LCD_DB4 = (data >> 4) & 0b1;
	LCD_DB5 = (data >> 5) & 0b1;
	LCD_DB6 = (data >> 6) & 0b1;
	LCD_DB7 = (data >> 7) & 0b1;

	LCD_ENABLE = HIGH;
	delay(15);
	LCD_ENABLE = LOW;
}

// Effacement
//Efface l'ecran et positionne le curseur à 0 (haut,gauche)
void lcd_clear(void)
{
	int i;
	for (i = 0; i < BUFFER_SIZE; i++)
	{
		g_display_buffer[i] = ' ';
	}
	lcd_write(CLEAR, 0);
}

//**Retour Curseur**
//Positionne le curseur à 0 (haut,gauche)
void lcd_home(void)
{
	lcd_write(HOME, 0);
}

//**Mode de décalage**
//Définit la direction de déplacement du curseur et si le texte doit suivre le curseur
//	ID : 1 = incremente adresse DD-RAM ; 0 = décremente
//	S  : 1 = Décale l' affichage; 0 = ne décale pas
void lcd_entry_mode_set(u8 id, u8 s)
{
	lcd_write(ENTRY_MODE_SET + 0x02 * id + 0x01 * s, 0);
}

//**Afficheur On/Off, curseur et clignotement**
//D : 1 = Affichage On ; 0 = Affichage Off
//C : 1 = Curseur en service ; 0 = Curseur Hors servic
//B : 1 = Curseur clignotant ; 0 = Curseur Fixe
void lcd_display_control(u8 d, u8 c, u8 b)
{
	lcd_write(DISPLAY_CONROL + 0x04 * d + 0x02 * c + 0x01 * b, 0);
}

void lcd_set_ddram_addr(u8 addr)
{
	addr &= 0x7f;
	lcd_write(SET_DDRAM + addr, 0);
}

//**Initialisation**
//DL : 1 = Adressage 8 bits ; 0 = Adressage 4 Bits
//N  : 1 = Afficheur 2 lignes ; 0 = Afficheur 1 ligne
//F  : 1 = Police de charactere 5X10 ; 0 = Police de charactere 5X8;

/*
 * tl;dr: initialise the screenm
 * 
 * 
 */

void lcd_function_set(u8 dl, u8 n, u8 f)
{
	lcd_write(FUNCTION_SET + 0x10 * dl + 0x08 * n + 0x04 * f, 0);
}

void lcd_put_char(u8 c)
{
	lcd_write(c, 1);
}


void lcd_put_str(u8 *str)
{
	u8 i = 0;
	while(str[i])
	{
		lcd_write(str[i++], 1);
	}
}

void lcd_put_hex(u8 c)
{
	lcd_put_char(hextoa(c >> 4));
	lcd_put_char(hextoa(c & 0x0F));
}

void lcd_put_hex_4bit(u8 c)
{
	lcd_put_char(hextoa(c & 0x0F));
}

void lcd_next_line()
{
	lcd_set_ddram_addr(0xc0);
}

void lcd_put_str_hex(u8 *str)
{
    while(*str)
    {
        lcd_put_hex(*str++);
    }
}

// Only < 10
void lcd_put_countdown(u8 c)
{
    lcd_set_ddram_addr(0xf);
    lcd_put_char(c + 48);
}

void put_tag(u8 *tag)
{
    u8 i;

	for(i = 0 ; i < 10 ; i++)
	{
		lcd_put_hex_4bit(tag[i]);
	}
}