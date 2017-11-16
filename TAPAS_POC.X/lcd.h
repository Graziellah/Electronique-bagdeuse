#include "main.h"

#ifndef LCD_H
#define LCD_H

#define LCD_RS LATFbits.LATF0
#define LCD_RW LATFbits.LATF2
#define LCD_ENABLE LATFbits.LATF3

#define LCD_DB0 LATEbits.LATE7
#define LCD_DB1 LATEbits.LATE6
#define LCD_DB2 LATEbits.LATE5
#define LCD_DB3 LATEbits.LATE4
#define LCD_DB4 LATEbits.LATE3
#define LCD_DB5 LATEbits.LATE2
#define LCD_DB6 LATEbits.LATE1
#define LCD_DB7 LATEbits.LATE0

#define LINE 2
#define CHAR 16
#define BUFFER_SIZE LINE*CHAR

u8	g_display_buffer[BUFFER_SIZE];

/* Control and display instructions
 
                                         RS  R/W  DB7  DB6  DB5  DB4  DB3  DB2  DB1  DB0  Execution time
Clear Display                             0    0    0    0    0    0    0    0    0    1          1.52ms
Return Home                               0    0    0    0    0    0    0    0    1    -          1.52ms
Entry Mode Set                            0    0    0    0    0    0    0    1  I/D    S            38us
Display ON/ OFF Control                   0    0    0    0    0    0    1    D    C    B            38us
Cursor or Display Shift                   0    0    0    0    0    1  S/C  R/L    -    -            38us
Function Set                              0    0    0    0    1   DL    N    F    -    -            38us
Set CGRAM Address                         0    0    0    1  AC5  AC4  AC3  AC2  AC1  AC0            38us
Set DDRAM Address                         0    0    1  AC6  AC5  AC4  AC3  AC2  AC1  AC0            38us
Read Busy Flag and Address Counter        0    1   BF  AC6  AC5  AC4  AC3  AC2  AC1  AC0
Write Data to RAM                         1    0   D7   D6   D5   D4   D3   D2   D1   D0            38us
Read Data from RAM                        1    1   D7   D6   D5   D4   D3   D2   D1   D0            38us

"-"   : Don't care
"I/D" : I / D = 1: Increment, I / D = 0: Decrement.
"S"   : S = 1: The display shift, S = 0: The display does not shift.
"D"   : D = 1: Display on, D = 0: Display off
"C"   : C = 1: Cursor on, C = 0: Cursor off
"B"   : B = 1: Blinks on, B= 0: Blinks off
"S/C" : Shift cursor. 0 = Shift cursor, 1 = Shift display
"R/L" : Shift direction. 0 = Shift left, 1 = Shift right
"DL"  : It sets interface data length. DL = 1: Data transferred with 8-bit length (DB7 - 0). DL = 0: Data transferred with 4-bit length (DB7 - 4).
"N"   : It sets the number of the display line. N = 0: One-line display. N = 1: Two-line display.
"F"   : It sets the character font. F = 0: 5 x 8 dots character font. F = 1: 5 x 10 dots character font.
"BF"  : Busy Flag. 1, it indicates the system is busy now and it will not accept any instruction until not busy (BF = 0). At the same time, the content of Address Counter (aaaaaaa)2 is read.
"ACX" : Adress counter
"DX"  : data
*/

#define CLEAR 0b00000001
#define HOME  0b00000010
#define ENTRY_MODE_SET 0b00000100
#define DISPLAY_CONROL 0b00001000
#define CURSOR_DISPLAY_SHIFT 0b00010000
#define FUNCTION_SET 0b00100000
#define SET_DDRAM 0b10000000

//Functions
void	lcd_write(u8 data, u8 rs);
u8      lcd_is_busy();
u8      lcd_read_data();
void	lcd_rw_mode(u8 enable);
void    lcd_put_char(u8 c);
void    lcd_put_str(u8* str);
void	lcd_init();
void	lcd_clear(void);
void	lcd_home(void);
void	lcd_entry_mode_set(u8 id, u8 s);
void	lcd_display_control(u8 d, u8 c, u8 b);
void	lcd_function_set(u8 dl, u8 n, u8 f);
void    lcd_set_ddram_addr(u8 addr);
void	display_write(u8 c, u8 l, u8 i);

#endif