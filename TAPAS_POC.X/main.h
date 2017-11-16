#include <sys/attribs.h>
//#include <plib.h> // Microchip PIC32MX Peripheral Library
#include <stdint.h>
#include <proc/p32mx340f512h.h>

#ifndef MAIN_H
#define MAIN_H

typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long int u32;

typedef char s8;
typedef int s16;
typedef long int s32;

extern u8 g_tag[];

extern u32 g_time_count;
extern u32 g_addr;

extern u8 g_timeout;
enum e_mode {read, add, delete};

u8 g_page[400][10];


#define SYSCLK (8000000UL)
#define BAUDRATE (9600)
#define INPUT 1
#define OUTPUT 0
#define HIGH 1
#define LOW 0
#define INVERT_LED (LATFINV = 0b10)
#define LED_OFF (LATFbits.LATF1 = 0)
#define LED_ON (LATFbits.LATF1 = 1)
#define READER_LED_RED_OFF (LATDbits.LATD2 = HIGH)
#define READER_LED_RED_ON (LATDbits.LATD2 = LOW)
#define RELAY_CLOSE (LATDbits.LATD4 = LOW)
#define RELAY_OPEN (LATDbits.LATD4 = HIGH)
#define READER_LED_GREEN_OFF (LATDbits.LATD3 = HIGH)
#define READER_LED_GREEN_ON (LATDbits.LATD3 = LOW)
#define READER_RED (READER_LED_GREEN_OFF)
#define READER_GREEN (READER_LED_RED_OFF)
#define READER_ORANGE READER_LED_GREEN_ON;  READER_LED_RED_ON;

extern const u8 g_master_tag_add[10]; // Tag de sebastien
extern const u8 g_master_tag_delete[10]; // Tag ?

#endif
