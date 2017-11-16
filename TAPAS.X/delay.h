#include "main.h"

#ifndef DELAY_H
#define DELAY_H

u32 g_time_count = 0;

void delay_init(void);
void delay(u32 time_ms);

#endif
