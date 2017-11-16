#include "main.h"

#ifndef UTIL_H
#define UTIL_H

/* Function */
u8 str_len(u8* str);
u8 str_cmp_size(const u8 *str1, const u8 *str2, u8 size);

u8 hextoa(u8 data);

u8 lrc(u8* data, u8 size);

void clean_tab(u8* tab, u8 size);

void add_bit(u8* tag, u8* tag_index, u8* nb_bit_read, u8* tag_lrc, u8 b);

#endif
