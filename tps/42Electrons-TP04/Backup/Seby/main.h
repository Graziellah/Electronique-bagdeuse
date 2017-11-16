/* 
 * File:   main.h
 * Author: bocal
 *
 * Created on March 21, 2017, 11:36 AM
 */

#ifndef MAIN_H
#define MAIN_H

// Includes
#include <xc.h>

// Typedefs
typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long int u32;

typedef char s8;
typedef int s16;
typedef long int s32;

// Button macros
#define BTN_PUSH_LIMIT 4
#define BTN_FULL_CYCLE 0x03
#define RELEASED_BTN_STATE 0x02
#define PUSHED_BTN_STATE 0x01
#define RESET_BTN_STATE 0x00
#define DEFAULT_BTN_STATE RELEASED_BTN_STATE
#define BTN_STATE PORTDbits.RD8

// LED macros
#define SET_LED_ON 0x00
#define SET_LED_OFF 0x01
#define LED_STATE_ON 0x01
#define LED_STATE_OFF 0x00
#define INVERT_LED_VALUE (LATFINV = 0x02)

// Pins macros
#define PIN_OUTPUT_MODE 0x00
#define PIN_INPUT_MODE 0x01

// Timer macros
#define TIME_CONST 3200

// Led Prototypes
u8              get_led_state(void);
u8              led_process(void);

void            led_blink_mode(void);
void            led_init(void);
void            led_turn_off(void);
void            led_turn_on(void);

// Time Prototypes
u16*            get_time_value(void);

void            set_time_value(u16 value);
void            timer_init(void);

#endif	/* MAIN_H */

