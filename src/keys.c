#include "fiflo.h"
#include "keys.h"

meta* add_chr_as_txt(char key, meta* Dt)
{
	if(Dt->chrs <= MAX_CHRS)
	{
		Dt->chrs++;
		CURR_LN_LEN++;

		// Fixes incremented value by terminator. TODO
		if(key == NTERM && Dt->chrs == 1)
		{
			Dt->chrs--;
			CURR_LN_LEN--;
		}
		Dt = add_mem_for_chrs(Dt);
		/* If the cursor is moved to the left and a char is inserted, rest of
		the text will be shifted to the right side. */
		if(Dt->cusr_x > 0)
		{
			Dt = shift_txt_horizonally('r', Dt);
		}
		CURR_LN[CURR_LN_LEN - Dt->cusr_x - NTERM_SZ] = key;
		CURR_LN[CURR_LN_LEN] = NTERM;
	}
	return Dt;
}

meta* linefeed(meta* Dt)
{
	if(Dt->lns < MAX_LNS)
	{
		Dt->lns++;

		// Enhance pointer that contains pointers to lines.
		Dt->txt = realloc(Dt->txt, sizeof(Dt->txt) * (Dt->lns + MEMBLK));
		chk_ptr(Dt->txt, "realloc array with lines\0", Dt);

		// Enhance pointer that contains lines length indicators.
		Dt->ln_len = realloc(Dt->ln_len, Dt->lns + MEMBLK);
		chk_ptr(Dt->ln_len, "realloc array with lines length\0", Dt);

		// The new line is allocated with only 2 bytes.
		CURR_LN = malloc(1 + NTERM_SZ);
		chk_ptr(CURR_LN, "2 bytes for initial line\0", Dt);

		// Naturally the new line doesn't contains any chars - only terminator.
		CURR_LN_LEN = 0;
		CURR_LN[CURR_LN_LEN] = NTERM;

		/* If user moved the cursor before enter, txt on the right will be
		moved to the new line. */
		if(Dt->cusr_x > 0)
		{
			for(term_t x = PRE_CURR_LN_LEN - Dt->cusr_x; x <= PRE_CURR_LN_LEN; x++)
			{
				CURR_LN[CURR_LN_LEN] = PRE_CURR_LN[x];
				CURR_LN_LEN++;
				Dt = add_mem_for_chrs(Dt);
			}
			PRE_CURR_LN[PRE_CURR_LN_LEN - Dt->cusr_x] = NTERM;
			CURR_LN[CURR_LN_LEN - 1] = NTERM;

			// Padding to the null terminator.
			CURR_LN_LEN--;
		}
	}
	return Dt;
}

meta* backspace(meta* Dt)
{
	if(Dt->chrs > 0)
	{
		// Prevent first char deletion in line.
		if(Dt->cusr_x != CURR_LN_LEN)
		{
			Dt = shift_txt_horizonally('l', Dt);
		}

		if(CURR_LN_LEN == 2)
		{
			// If there are: char + terminator, shrink to 2 bytes.
			CURR_LN = realloc(CURR_LN, 1 + NTERM_SZ);
		}
		else if(CURR_LN_LEN == MEMBLK)
		{
			CURR_LN = realloc(CURR_LN, MEMBLK);
		}
		else if(CURR_LN_LEN > MEMBLK && CURR_LN_LEN % MEMBLK == 0)
		{
			// There is more memory needed.
			CURR_LN = realloc(CURR_LN, CURR_LN_LEN);
		}
		chk_ptr(CURR_LN, "shrink memblock for the current line\0", Dt);

		if(Dt->cusr_x != (CURR_LN_LEN + INDEX))
		{
			CURR_LN_LEN--;
			Dt->chrs--;
		}
		CURR_LN[CURR_LN_LEN] = NTERM;
	}
	return Dt;
}

meta* ctrl_h(meta* Dt)
{
	// Cursor can be moved right if is shifted left. 0 - default right position.
	if(Dt->cusr_x > 0)
	{
		// Move the cursor right.
		Dt->cusr_x--;
	}
	return Dt;
}

meta* ctrl_g(meta* Dt)
{
	// Move only when the cursor isn't at the start of the line.
	if(Dt->cusr_x < CURR_LN_LEN)
	{
		// Move the cursor left.
		Dt->cusr_x++;
	}
	return Dt;
}

