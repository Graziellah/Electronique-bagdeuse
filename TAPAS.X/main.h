#include <sys/attribs.h>
#include <plib.h> // Microchip PIC32MX Peripheral Library
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

u8 g_timout = 9;
enum e_mode {read, add, delete};

#define SYSCLK (8000000UL)
#define BAUDRATE (9600) //UART
#define INPUT 1
#define OUTPUT 0
#define HIGH 1
#define LOW 0

const u8 g_master_tag_add[] = {0x6, 0xF, 0x0, 0x0, 0x3, 0x5, 0x3, 0x7, 0x8, 0x7}; // Tag de sebastien
const u8 g_master_tag_delete[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9}; // Tag ?

#endif
