int tag_lu();
int check_lrc();
int nb_bit_lu;

enum e_mode {read, add, delete};
enum e_mode g_mode = read;

//0000 0000 0000 0000 0000 0000 0000 0000 0000 0000

int search_in_flash(tag, size)
{
	int *addr = 0;

	while(size--)
	{
		read_flash(addr, buffer, 10);
		if(!strcmp(tag_lu, buffer))
		{
			return(1)
		}
		addr += 10;
	}
}


if (nb_bit_lu == 44)
{
	if(check_lrc())
	{
		if(mod == add && tag_is_not_master_add && tag_is_not_master_delete)
		{
			if (found_tag_in_flash())
			{
				// tag already existe in memories
			}
			else
			{
				// ajouter le tag dans la flash
			}

			mode = 0;
		}

		else if (mod == delete && tag_is_not_master_add && tag_is_not_master_delete)
		{
			if (found_tag_in_flash())
			{
				// remove tag from flash
			}
			else
			{
				// tag doesn't exist in flash
			}
			mode = 0;
		}

		if(tag_is_master_add())
		{
			mode = add;
		}
		else if (tag_is_master_delete())
		{
			mode = delete;
		}
		else
		{
			if(found_tag_in_flash())
			{
				led_verte();
				activer_relay();
				delay();
				desactiver_relay();
				led_orange();
			}
			else
			{
				led_rouge();
				delay();
				led_orange();
			}
		}
	}
	else
	{
		//error LRC
	}
}

void put_tag(u8 *tag)
{
	for(i = 0 ; i < 10 ; i++)
	{
		lcd_put_hex_4bit(tag[i]);
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

int main()
{
	SYSTEMConfigPerformance(SYSCLK);

	uart2_init();
	spi2_init();
	init_io();
	init_timer();
	init_int();
	lcd_init();

	while (1)
	{

		if(g_nb_bit_read == 44)
		{
			if(g_tag_lrc == lrc(g_tag, 10))
			{

				if (mode == add && !is_master_tag(g_tag))
				{
					// add
					mode = read;
				}

				else if (mode == delete && !is_master_tag(g_tag))
				{
					//delete
					mode = read;
				}

				else if (mode == read)
				{
					mode = get_tag_mode(g_tag);
					if (mode == read)
					{
						// checker si auth
						lcd_clear();
						lcd_put_str("Tag read ");
						lcd_next_line();
						put_tag(g_tag);
					}
					else if (mode == add)
					{
						lcd_clear();
						lcd_put_str("Add mode ");
					}
					else if (mode == delete)
					{
						lcd_clear();
						lcd_put_str("Delete mode ");
					}
				}
			}

			else
			{
				lcd_clear();
				lcd_put_str("LRC Error");
				mode = read;
			}

			g_nb_bit_read = 0;

		}

		WDTCONbits.WDTCLR = 1;	// Clear watchdog
	}
	return (0);
}
