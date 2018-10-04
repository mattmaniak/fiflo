#include "api.h"

meta* dealloc_block(meta* dt)
{
	_Bool line_back = 0;
	if(dt->ln_len - 1 % MEMBLK == MEMBLK - 1)
	{
		CURR_LN = realloc(CURR_LN, (2 * dt->ln_len) - MEMBLK);
		check_ptr(dt, CURR_LN, "free the memblock with a line\0");
	}
	if(dt->ln_len > 0 && dt->cusr_x != (dt->ln_len + INDEX))
	{
		dt->ln_len--;
		dt->chrs--;
	}
	else if(dt->ln_len == 0)
	{
		line_back = 1;
		dt->cusr_y = 0;
	}
	CURR_LN[dt->ln_len] = NTERM;

	if(line_back == 1 && dt->lns > 0 && LN_ABOVE[strlen(LN_ABOVE) - NTERM_SZ] == LF)
	{
		free(CURR_LN);
		dt->lns--;
		dt->ln_len = (buf_t) strlen(CURR_LN) - NTERM_SZ;
		CURR_LN[dt->ln_len] = NTERM;
		if(dt->chrs > 0) // Just for the LF removal.
		{
			dt->chrs--;
		}
	}
	return dt;
}

meta* alloc_block(meta* dt, char mode)
{
	switch(mode)
	{
		case 'c':
			dt->chrs++;
			dt->ln_len++;
			if(dt->ln_len % MEMBLK == 0) // MEMBLK - 1 chars + NTERM -> alloc.
			{
				CURR_LN = realloc(CURR_LN, dt->ln_len + MEMBLK);
				check_ptr(dt, CURR_LN, "alloc new memblock for chars\0");
			}
			break;

		case 'l':
			if(dt->lns++ >= MAX_LNS)
			{
				dt->lns = MAX_LNS;
			}
			else
			{
				if(dt->lns % MEMBLK == 0) // Allocates with a one line reserve.
				{
					dt->txt =
					realloc(dt->txt, sizeof(dt->txt) * (dt->lns + MEMBLK));
				}
				CURR_LN = malloc(MEMBLK);
				check_ptr(dt, CURR_LN, "alloc byte in the new line\0");
			}
			CURR_LN[dt->ln_len = 0] = NTERM;
			break;
	}
	return dt;
}

meta* shift_txt(meta* dt, char direction)
{
	switch(direction)
	{
		case 'l':
			if(dt->cusr_x > dt->ln_len - 1 && dt->ln_len > 0)
			{
				dt->cusr_x = dt->ln_len - 1;
			}
			if(dt->ln_len > 0)
			{
				for(term_t x = dt->ln_len - dt->cusr_x; x <= dt->ln_len; x++)
				{
					CURR_LN[x - 1] = CURR_LN[x];
				}
			}
			break;

		case 'r':
			for(term_t x = dt->ln_len; x >= dt->ln_len - dt->cusr_x; x--)
			{
				CURR_LN[x] = CURR_LN[x - 1];
			}
			break;
	}
	return dt;
}

meta* add_char(meta* dt, char key)
{
	if(dt->chrs <= MAX_CHRS)
	{
		dt = alloc_block(dt, 'c');
		if(dt->cusr_x > 0)
		{
			shift_txt(dt, 'r');
		}
		dt->txt[dt->lns - dt->cusr_y][LAST_CHAR - dt->cusr_x] = key;
		dt->txt[dt->lns - dt->cusr_y][dt->ln_len] = NTERM;

		// Initializer. TODO
		if(key == NTERM && dt->chrs > 0)
		{
			dt->chrs--;
			dt->ln_len--;
		}

		if(key == LF)
		{
			dt = alloc_block(dt, 'l');
			if(dt->cusr_x > 0)
			{
				for(term_t x = (term_t) (strlen(LN_ABOVE) - dt->cusr_x); x <= strlen(LN_ABOVE); x++)
				{
					CURR_LN[dt->ln_len++] = LN_ABOVE[x];
				}
				LN_ABOVE[strlen(LN_ABOVE) - dt->cusr_x] = NTERM;
				dt->cusr_x = dt->ln_len;
			}
		}
	}
	return dt;
}

meta* recognize_char(meta* dt, char key) // TODO: KEYMAP.
{
	// Prevent inputting ANSI escape sequences.
	if(key != ESCAPE)
	{
		switch(key)
		{
			default:
				dt = add_char(dt, key);
				break;

			// Pipe and signal prevention. TODO: FULL UPPER BAR FLUSH.
			case NEG:
				free_all_exit(dt, 0);

			case CTRL_D:
				save_file(dt);
				break;

			case CTRL_H:
				dt = ctrl_h(dt);
				break;

			case CTRL_G:
				dt = ctrl_g(dt);
				break;

			case CTRL_Y:
				dt = ctrl_y(dt);
				break;

			case CTRL_B:
				dt = ctrl_b(dt);
				break;

			case BACKSPACE:
				dt = backspace(dt);
				break;
		}
	}
	// DEBUG
	printf("last: %d cusr_x: %d cusr_y: %d\n", key, dt->cusr_x, dt->cusr_y);
	return dt;
}

