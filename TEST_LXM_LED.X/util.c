#include "util.h"

u8 str_len(u8* str)
{
	u8 i = 0;

	while(str[i])
	{
		i++;
	}
	return(i);
}

u8 str_cmp_size(const u8 *str1, const u8 *str2, u8 size)
{
	while (*str1 == *str2 && size)
	{
		str1++;
		str2++;
        size--;
	}
	return (*str1 - *str2);
}

u8 hextoa(u8 data)
{
	data += 0x30;
	if (data>0x39) data+=7;
	return data;
}

u8 lrc(u8* data, u8 size)
{
	u8 lrc = 0;
	while(size)
	{
		lrc ^= *data;
        data++;
		size--;
	}
	return(lrc);
}

void clean_tab(u8* tab, u8 size)
{
	u8 i = 0;

	for(i = 0 ; i < size ; i++)
	{
		tab[i] = 0;
	}
}

void add_bit(u8* tag, u8* tag_index, u8* nb_bit_read, u8* tag_lrc, u8 b)
{
	if (*nb_bit_read == 0)
	{
		clean_tab(tag, 10);
        *tag_index = 0;
        *tag_lrc = 0;
	}

	*nb_bit_read += 1;

	if (*nb_bit_read <= 40)
	{
		if (*nb_bit_read % 4 == 1 && *nb_bit_read > 1)
		{
			*tag_index +=1;
		}
        
        tag[*tag_index] <<= 1;
                
        if (b)
		{
            tag[*tag_index]+= 1;
		}
	}
	else if (*nb_bit_read > 40)
	{
		if (*nb_bit_read % 4 == 1)
		{
			*tag_lrc = 0;
		}
        
        *tag_lrc <<= 1;

		if (b)
		{	
			*tag_lrc += 1;
		}
	}
}


