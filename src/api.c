#include "api.h"

meta* dealloc_block(meta* Dat)
{
	_Bool line_back = 0;
	if(Dat->ln_len[Dat->lns] - 1 % MEMBLK == MEMBLK - 1)
	{
		CURR_LN = realloc(CURR_LN, (2 * Dat->ln_len[Dat->lns]) - MEMBLK);
		check_ptr(Dat, CURR_LN, "free the memblock with a line\0");
	}
	if(Dat->ln_len[Dat->lns] > 0 && Dat->cusr_x != (Dat->ln_len[Dat->lns] + INDEX))
	{
		Dat->ln_len[Dat->lns]--;
		Dat->chrs--;
	}
	else if(Dat->ln_len[Dat->lns] == 0)
	{
		line_back = 1;
		Dat->cusr_y = 0;
	}
	CURR_LN[Dat->ln_len[Dat->lns]] = NTERM;

	if(line_back == 1 && Dat->lns > 0 && LN_ABOVE[strlen(LN_ABOVE) - NTERM_SZ] == LF)
	{
		free(CURR_LN);
		CURR_LN = NULL;
		Dat->lns--;
		Dat->ln_len[Dat->lns] = (buf_t) strlen(CURR_LN) - NTERM_SZ;
		CURR_LN[Dat->ln_len[Dat->lns]] = NTERM;
		if(Dat->chrs > 0) // Just for the LF removal.
		{
			Dat->chrs--;
		}
	}
	return Dat;
}

meta* alloc_block(meta* Dat, char mode)
{
	switch(mode)
	{
		case 'c':
			Dat->chrs++;
			Dat->ln_len[Dat->lns]++;
			if(Dat->ln_len[Dat->lns] % MEMBLK == 0) // MEMBLK - 1 chars + NTERM -> alloc.
			{
				CURR_LN = realloc(CURR_LN, Dat->ln_len[Dat->lns] + MEMBLK);
				check_ptr(Dat, CURR_LN, "alloc new memblock for chars\0");
			}
			break;

		case 'l':
			if(Dat->lns++ >= MAX_LNS)
			{
				Dat->lns = MAX_LNS;
			}
			else
			{
				if(Dat->lns % MEMBLK == 0) // Allocates with a one line reserve.
				{
					Dat->txt =
					realloc(Dat->txt, sizeof(Dat->txt) * (Dat->lns + MEMBLK));
				}
				CURR_LN = malloc(MEMBLK);
				check_ptr(Dat, CURR_LN, "alloc byte in the new line\0");
			}
			CURR_LN[Dat->ln_len[Dat->lns] = 0] = NTERM;
			break;
	}
	return Dat;
}

meta* shift_txt(meta* Dat, char direction)
{
	switch(direction)
	{
		case 'l':
			if(Dat->cusr_x > Dat->ln_len[Dat->lns] - 1 && Dat->ln_len[Dat->lns] > 0)
			{
				Dat->cusr_x = Dat->ln_len[Dat->lns] - 1;
			}
			if(Dat->ln_len[Dat->lns] > 0)
			{
				for(term_t x = Dat->ln_len[Dat->lns] - Dat->cusr_x; x <= Dat->ln_len[Dat->lns]; x++)
				{
					CURR_LN[x - 1] = CURR_LN[x];
				}
			}
			break;

		case 'r':
			for(term_t x = Dat->ln_len[Dat->lns]; x >= Dat->ln_len[Dat->lns] - Dat->cusr_x; x--)
			{
				CURR_LN[x] = CURR_LN[x - 1];
			}
			break;
	}
	return Dat;
}

meta* add_char(meta* Dat, char key)
{
	if(Dat->chrs <= MAX_CHRS)
	{
		Dat = alloc_block(Dat, 'c');
		if(Dat->cusr_x > 0)
		{
			shift_txt(Dat, 'r');
		}
		Dat->txt[Dat->lns - Dat->cusr_y][Dat->ln_len[Dat->lns] - NTERM_SZ - Dat->cusr_x] = key;
		Dat->txt[Dat->lns - Dat->cusr_y][Dat->ln_len[Dat->lns]] = NTERM;

		// Initializer. TODO
		if(key == NTERM && Dat->chrs > 0)
		{
			Dat->chrs--;
			Dat->ln_len[Dat->lns]--;
		}

		if(key == LF)
		{
			Dat = alloc_block(Dat, 'l');
			if(Dat->cusr_x > 0)
			{
				for(term_t x = (term_t) (strlen(LN_ABOVE) - Dat->cusr_x); x <= strlen(LN_ABOVE); x++)
				{
					CURR_LN[Dat->ln_len[Dat->lns]++] = LN_ABOVE[x];
				}
				LN_ABOVE[strlen(LN_ABOVE) - Dat->cusr_x] = NTERM;
				Dat->cusr_x = Dat->ln_len[Dat->lns];
			}
		}
	}
	return Dat;
}

meta* recognize_char(meta* Dat, char key) // TODO: KEYMAP.
{
	// Prevent inputting ANSI escape sequences.
	if(key != ESCAPE)
	{
		switch(key)
		{
			// Pipe and signal prevention. TODO: FULL UPPER BAR FLUSH.
			case NEG:
				free_all_exit(Dat, 0);

			default:
				Dat = add_char(Dat, key);
				break;

			case BACKSPACE:
				Dat = backspace(Dat);
				break;

			case CTRL_D:
				save_file(Dat);
				break;

			case CTRL_H:
				Dat = ctrl_h(Dat);
				break;

			case CTRL_G:
				Dat = ctrl_g(Dat);
				break;

			case CTRL_Y:
				Dat = ctrl_y(Dat);
				break;

			case CTRL_B:
				Dat = ctrl_b(Dat);
				break;
		}
	}
	// DEBUG
	printf("last: %d cusr_x: %d cusr_y: %d\n", key, Dat->cusr_x, Dat->cusr_y);
	return Dat;
}

