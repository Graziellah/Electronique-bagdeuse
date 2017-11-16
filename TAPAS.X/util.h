#include "main.h"

#ifndef UTIL_H
#define UTIL_H

/* Function */
u8 str_len(u8* str);
u8 hextoa(u8 data);
void add_bit(u8* tag, u8* tag_index, u8* nb_bit_read, u8* tag_lrc, u8 b);
void clean_tab(u8* tab, u8 size);
u8 str_cmp_size(const u8 *str1, const u8 *str2, u8 size);
void enable_timout();
void disable_timout();

#endif
