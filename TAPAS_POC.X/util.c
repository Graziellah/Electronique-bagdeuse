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

s8 str_cmp_size(const u8 *str1, const u8 *str2, u8 size)
{
	while (*str1 == *str2 && size)
	{
		str1++;
		str2++;
        size--;
	}
	return (*str1 - *str2);
}

s8 tag_cmp(const u8 *tag1, const u8 *tag2)
{
    u8 i = 10;
 
	while (*tag1 == *tag2 && i)
	{
		tag1++;
		tag2++;
        i--;
	}
	return (i);
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


enum e_mode get_tag_mode(u8 *tag)
{
	enum e_mode mode = read;

	if (!str_cmp_size(tag, g_master_tag_delete, 10))
	{
		mode = delete;
	}
	else if (!str_cmp_size(tag, g_master_tag_add, 10))
	{
		mode = add;
	}

	return(mode);
}

u8 is_master_tag(u8 *tag)
{
	if (!str_cmp_size(tag, g_master_tag_add, 10) || !str_cmp_size(tag, g_master_tag_delete, 10))
	{
		return(1);
	}
	return(0);
}

u8 is_master_tag_add(u8 *tag)
{
	if (!str_cmp_size(tag, g_master_tag_add, 10))
	{
		return (1);
	}
	return(0);
}

u8 is_master_tag_delete(u8 *tag)
{
	if (!str_cmp_size(tag, g_master_tag_delete, 10))
	{
		return (1);
	}
	return(0);
}

void disable_timout()
{
    T1CONbits.ON = 0;
    TMR1 = 0;
    g_timeout = 9;
}

void enable_timout()
{
    g_timeout = 9;
    lcd_put_countdown(g_timeout);
    TMR1 = 0;
    T1CONbits.ON = 1;
}